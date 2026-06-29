// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
#include <sem/DriftModelGenerator.h>
#include <sem/DataModel.h>
#include <sml/Vector.h>
#include <numbers>
#include <cmath>

namespace sem
{
DriftModelGenerator::DriftModelGenerator(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& weatherStation, bool useEngineRepair, bool useAnchoring)
	: m_weatherstationScheme(weatherStation)
	, m_currentTable(getCurrentTable(areaModel, weatherStation))
	, m_tideTable(getTideTable(areaModel, weatherStation))
	, m_windStrengthTable(getWindStrengthTable(areaModel, weatherStation))
	, m_tideAngle(weatherStation.getTideAngle())
	, m_tidePeriod(getTidePeriod())
	, m_numTideSamples(GeneralModelData::numTideSamples())
	, m_useEngineRepair(useEngineRepair)
	, m_useAnchoring(useAnchoring)
{
	size_t numScales = m_windStrengthTable.size();
	// Create angular distribution functions for the winddirections from the database entries
	const auto& winddirectionTables = areaModel.getWinddirectionTables();
	size_t numWindDirections;
	for (size_t iScale = 0; iScale < numScales; iScale++) {
		auto it = winddirectionTables.find(m_windStrengthTable[iScale]->getDirectionTableUid());
		if (it == winddirectionTables.end()) {
			LogError("Wind direction table" << m_windStrengthTable[iScale]->getDirectionTableUid() << " not found. ");
		}

		std::vector<double> angles;
		std::vector<double> probabilities;
		for (const auto& scheme : it->second) {
			angles.push_back(scheme->getAngle());
			probabilities.push_back(scheme->getProbability());
		}
		m_pWindDirections[iScale] = sml::AngularDistributionFunction(angles, probabilities, true);
	}

	// Evaluate the tide velocities
	m_tideVelocities.resize(m_numTideSamples, {});
	if (m_tidePeriod != 0.0) {
		LogInfo("Tide period is " << m_tidePeriod);
		double stepSize = m_tidePeriod / m_numTideSamples;	// Steps in the starting position of the beat frequency
		for (size_t tideIndex = 0; tideIndex < m_numTideSamples; tideIndex++) {
			double t0 = stepSize * tideIndex;
			auto& vtide = m_tideVelocities[tideIndex];
			for (size_t nHour = 0; nHour < 24; nHour++) {
				double t = t0 + 3600 * nHour;
				vtide[nHour] = computeTideVelocity(t);
			}
		}
	}
}



void DriftModelGenerator::compute(DriftModel& driftModel, const std::string uid, const double draft, const double height) const
{
	std::vector<double>& allAngles = driftModel.m_discretisation;

	// Ship specific parameters
	driftModel.m_shipCategoryUid = uid;
	driftModel.m_weatherStationUid = m_weatherstationScheme.getUID();
	double elevationMin = -draft;
	double elevationMax = elevationMin + height;


	// First compute maximum number of drift angles so that enough memory can be allocated
	// TODO this can be done in a neater way
	size_t maxNumDir = 0;
	size_t numScales = m_windStrengthTable.size();
	driftModel.m_numScales = numScales;
	for (size_t iCurrent = 0; iCurrent < m_currentTable.size(); iCurrent++) {
		// Get the current information
		const auto& currentScheme = m_currentTable[iCurrent];
		const double currentVelocity = currentScheme->getVelocity();
		const double currentAngle = currentScheme->getAngle();
		sml::Vector2d eCurrent(std::cos(currentAngle), std::sin(currentAngle));
		sml::Vector2d vCurrent = eCurrent * currentVelocity;
		if (currentScheme->getProbability() == 0) {
			LogWarning("The current scheme " << currentScheme->getUID() << " from current table " << currentScheme->getTableUid() << " has a zero probability.");
		}
		for (size_t iScale = 0; iScale < numScales; iScale++) {
			// Get the wind information
			const auto& windstrengthScheme = m_windStrengthTable[iScale];
			const size_t scale = windstrengthScheme->getScale();

			// Ship-specific drag velocity in the direction of the wind
			double dragVelocity = computeDragVelocity(scale, windstrengthScheme->getWaveHeight(), elevationMin, elevationMax);

			// Find the theoretical limits of the drift angles [thetaMin, thetaMax] and discretise the bounds
			double r = currentVelocity / (dragVelocity + 1e-8);
			double a0lim = std::sqrt(1.0 - 1.0 / (r * r));
			sml::AngleBounds driftBounds = computeDriftBounds(r, a0lim, currentAngle);
			std::vector<double> driftAngles = computeDriftAngles(driftBounds);
			maxNumDir = driftAngles.size() > maxNumDir ? driftAngles.size() : maxNumDir;
		}
	}

	// Allocate enough memory
	driftModel.resize(m_currentTable.size(), maxNumDir, m_numTideSamples);
	driftModel.m_discretisation.clear();


	// Cache all the relevant data which can be pre-computed before heading into the exposure computation
	for (size_t iCurrent = 0; iCurrent < m_currentTable.size(); iCurrent++) {
		// Get the current information
		const auto& currentScheme = m_currentTable[iCurrent];
		const double currentVelocity = currentScheme->getVelocity();
		const double currentAngle = currentScheme->getAngle();
		sml::Vector2d eCurrent(std::cos(currentAngle), std::sin(currentAngle));
		sml::Vector2d vCurrent = eCurrent * currentVelocity;
		driftModel.m_pCurrent[iCurrent] = currentScheme->getProbability();
		if (currentScheme->getProbability() == 0) {
			LogWarning("The current scheme " << currentScheme->getUID() << " from current table " << currentScheme->getTableUid() << " has a zero probability.");
		}
		for (size_t iScale = 0; iScale < numScales; iScale++) {
			// Get the wind information
			const auto& windstrengthScheme = m_windStrengthTable[iScale];
			const size_t scale = windstrengthScheme->getScale();
			const auto windDirections = m_pWindDirections[iScale];
			driftModel.m_pScale[iScale] = windstrengthScheme->getProbability();
			driftModel.m_scale[iScale] = scale;

			// Ship-specific drag velocity in the direction of the wind
			double dragVelocity = computeDragVelocity(scale, windstrengthScheme->getWaveHeight(), elevationMin, elevationMax);

			// Find the theoretical limits of the drift angles [thetaMin, thetaMax] and discretise the bounds
			double r = currentVelocity / (dragVelocity + 1e-8);
			double a0lim = std::sqrt(1.0 - 1.0 / (r * r));
			driftModel.m_r[iCurrent][iScale] = r;
			sml::AngleBounds driftBounds = computeDriftBounds(r, a0lim, currentAngle);
			std::vector<double> driftAngles = computeDriftAngles(driftBounds);

			// Compute windAngles
			std::vector<double> windAngle0(driftAngles.size());
			std::vector<double> windAngle1(driftAngles.size());
			for (size_t i = 0; i < driftAngles.size(); i++) {
				sml::Vector2d eDrift(std::cos(driftAngles[i]), std::sin(driftAngles[i]));
				double a0 = eCurrent * eDrift;
				double a1sqr = std::max(0.0, r * r * (a0 * a0 - 1) + 1);
				double a1 = std::sqrt(a1sqr);
				sml::Vector2d eWind0 = eDrift * (r * a0 + a1) - eCurrent * r;
				windAngle0[i] = sml::normalisedPositiveAngles(std::atan2(eWind0[1], eWind0[0]));
				if (r > 1) {
					sml::Vector2d eWind1 = eDrift * (r * a0 - a1) - eCurrent * r;
					windAngle1[i] = sml::normalisedPositiveAngles(std::atan2(eWind1[1], eWind1[0]));
				}
			}
			
			// Compute the effective velocities and probability densities on drift angle
			size_t numbins = driftAngles.size() - 1;
			std::vector<double> probabilityDensity0(numbins, 0);
			std::vector<double> probabilityDensity1(numbins, 0);
			std::vector<double> ve0(numbins);
			std::vector<double> ve1(numbins);
			double sum0 = 0.0;
			double sum1 = 0.0;
			for (size_t i = 0; i < numbins; i++) {
				size_t iNext = i + 1;
				double binAngle = 0.5 * (driftAngles[i] + driftAngles[iNext]);
				if (!driftBounds.isInRange(binAngle)) {
					continue;
				}
				double binSize = driftAngles[iNext] - driftAngles[i];
				sum0 += windDirections.integrate(windAngle0[i], windAngle0[iNext]);
				probabilityDensity0[i] = windDirections.integrate(windAngle0[i], windAngle0[iNext]) / binSize;
				sml::Vector2d eBin(std::cos(binAngle), std::sin(binAngle));
				double a0 = eCurrent * eBin;
				double a1sqr = std::max(0.0, r * r * (a0 * a0 - 1) + 1); // By construction this angle should be valid, max is to guard for numerical artifacts
				double a1 = std::sqrt(a1sqr);
				ve0[i] = (r * a0 + a1) * dragVelocity;
				if (r > 1) {
					probabilityDensity1[i] = windDirections.integrate(windAngle1[iNext], windAngle1[i]) / binSize; // Have to integrate in reverse order
					sum1 += windDirections.integrate(windAngle1[iNext], windAngle1[i]);
					ve1[i] = (r * a0 - a1) * dragVelocity;
				}
			}
			if (numbins > 0 && std::abs((sum0 + sum1) - 1.0) > 1e-3) { // Is this the fix??
				LogError("Internal error, not all wind directions are used in the drift direction probability");
			}

			// Perform tide simulations and get the minimum and maximum distances
			computeTrajectories(driftModel, driftAngles, ve0, ve1, iCurrent, iScale);
			computeTrajectoryExtrema(driftModel.m_maxTideData0, driftModel.m_maxTrajectories0, iCurrent, iScale, numbins, m_numTideSamples);
			computeTrajectoryExtrema(driftModel.m_minTideData0, driftModel.m_minTrajectories0, iCurrent, iScale, numbins, m_numTideSamples);
			computeTrajectoryExtrema(driftModel.m_maxTideData1, driftModel.m_maxTrajectories1, iCurrent, iScale, numbins, m_numTideSamples);
			computeTrajectoryExtrema(driftModel.m_minTideData1, driftModel.m_minTrajectories1, iCurrent, iScale, numbins, m_numTideSamples);

			// Generate angular distribution functions
			driftAngles.pop_back(); // Remove the 2 pi entry as that is implicit in angular distribution
			driftModel.m_pDriftAngle0[iCurrent][iScale] = sml::AngularDistributionFunction(driftAngles, probabilityDensity0, false);
			driftModel.m_pDriftAngle1[iCurrent][iScale] = sml::AngularDistributionFunction(driftAngles, probabilityDensity1, false);

			allAngles.insert(allAngles.end(), driftAngles.begin(), driftAngles.end());

			// Check that the sum is one
			double totalArea = driftModel.m_pDriftAngle0[iCurrent][iScale].getTotalArea() + driftModel.m_pDriftAngle1[iCurrent][iScale].getTotalArea();
			if (abs(totalArea - 1) > 1e-3) {
				LogError("Internal error. Drift angle distribution is not correctly computed. Value of " << abs(totalArea - 1) << " should be close to zero.");
			}
		}
	}

	// Sort out all angles to generate a discretisation
	std::sort(allAngles.begin(), allAngles.end());
	allAngles.erase(std::unique(allAngles.begin(), allAngles.end()), allAngles.end());

	// Enable/disable measures
	driftModel.m_useAnchoring = m_useAnchoring;
	driftModel.m_useEngineRepair = m_useEngineRepair;
}



double DriftModelGenerator::computeDragVelocity(size_t scale, double zeta, double elevationMin, double elevationMax) const
{
	const double tp = GeneralModelData::get().getPeakPeriod(scale);
	constexpr double factor = 4.0 * std::numbers::pi * std::numbers::pi;
	constexpr double hullDragCoefficient = GeneralModelData::hullDragCoefficient();
	constexpr double windDragCoefficient = GeneralModelData::windDragCoefficient();
	constexpr double rhoAir = GeneralModelData::rhoAir();
	constexpr double rhoWater = GeneralModelData::rhoWater();
	constexpr double densityRatio = rhoAir / rhoWater;
	constexpr double g = GeneralModelData::g();
	const double waveNumber = factor / (tp * tp * GeneralModelData::g());
	const double kT = -waveNumber * elevationMin;
	double driftCoefficient = 1.0;
	if (kT <= 1) {
		driftCoefficient = -1.4736 * kT * kT * kT + 2.4765 * kT * kT - 0.0315 * kT;
	}
	double windElevation = std::max(0.0, elevationMax); // Submerged ships dont have any wind interaction
	double windSpeed = GeneralModelData::get().getWindSpeed(scale);
	double vDrag = std::sqrt(1.0 / (-elevationMin * hullDragCoefficient) * (densityRatio * windElevation * windDragCoefficient * windSpeed * windSpeed + 1.0 / 8.0 * g * zeta * zeta * driftCoefficient * driftCoefficient));
	return vDrag;
}



sml::AngleBounds DriftModelGenerator::computeDriftBounds(const double r, const double a0lim, const double angle) const
{
	sml::AngleBounds driftBounds;
	if (r <= 1) {
		driftBounds.setLowerBound(0.0);
		driftBounds.setUpperBound(2.0 * std::numbers::pi);
		driftBounds.setExtent(2.0 * std::numbers::pi);
	}
	if (r > 1) {
		double halfExtent = std::max(std::acos(a0lim), 1e-5); // Regularise such that there is always some small extent
		driftBounds.setLowerBound(sml::normalisedPositiveAngles(angle - halfExtent));
		driftBounds.setUpperBound(sml::normalisedPositiveAngles(angle + halfExtent));
		driftBounds.setExtent(2.0 * halfExtent);
	}
	LogVerbose("r=" << r << ", lowerBoud=" << driftBounds.getLowerBound() << ", upperBound=" << driftBounds.getUpperBound());
	return driftBounds;
}



std::vector<double> DriftModelGenerator::computeDriftAngles(const sml::AngleBounds& driftBounds) const
{
	// Discretise the driftBounds and generate angles
	std::vector<double> driftAngles;
	size_t numBins = std::ceil(driftBounds.getExtent() / GeneralModelData::maxAngularBinSize());
	double binSize = driftBounds.getExtent() / numBins;
	for (size_t i = 0; i < numBins; i++) {
		double angle = sml::normalisedPositiveAngles(driftBounds.getLowerBound() + i * binSize);
		if (angle > 0.0) {
			driftAngles.push_back(angle);
		}
	}

	// Explicitly set the upper bound if it is not a domain boundary, then sort and add the domain boundaries
	double upperBound = driftBounds.getUpperBound();
	if ((upperBound != 0.0) && (upperBound != 2.0 * std::numbers::pi)) {
		driftAngles.push_back(sml::normalisedPositiveAngles(driftBounds.getUpperBound()));
	}
	std::sort(driftAngles.begin(), driftAngles.end());
	driftAngles.insert(driftAngles.begin(), 0.0);
	driftAngles.push_back(2.0 * std::numbers::pi);

	return driftAngles;
}



void DriftModelGenerator::computeTrajectories(DriftModel& driftModel, const std::vector<double>& driftAngles,
	const std::vector<double>& ve0, const std::vector<double>& ve1, const size_t iCurrent, const size_t iScale) const
{
	double stepSize = m_tidePeriod / m_numTideSamples;
	for (size_t i = 0; i < driftAngles.size() - 1; i++) {
		double binAngle = 0.5 * (driftAngles[i] + driftAngles[i + 1]);
		const double tideFactor = std::cos(binAngle) * std::cos(m_tideAngle) + std::sin(binAngle) * std::sin(m_tideAngle); // Projection factor of tide velocity on drift direction
		for (size_t iSample = 0; iSample < m_numTideSamples; iSample++) {
			auto& maxDistances0 = driftModel.m_maxTrajectories0[iCurrent][iScale][i][iSample].distances;
			auto& maxDistances1 = driftModel.m_maxTrajectories1[iCurrent][iScale][i][iSample].distances;
			auto& minDistances0 = driftModel.m_minTrajectories0[iCurrent][iScale][i][iSample].distances;
			auto& minDistances1 = driftModel.m_minTrajectories1[iCurrent][iScale][i][iSample].distances;
			auto& tideVelocity = m_tideVelocities[iSample];
			maxDistances0[0] = 0;
			maxDistances1[0] = 0;
			minDistances0[0] = 0;
			minDistances1[0] = 0;
			for (size_t hour = 1; hour < 24; hour++) { // Simulate a full day
				maxDistances0[hour] = maxDistances0[hour - 1] + (ve0[i] + tideVelocity[hour - 1] * tideFactor) * 3600;
				maxDistances1[hour] = maxDistances1[hour - 1] + (ve1[i] + tideVelocity[hour - 1] * tideFactor) * 3600;
				minDistances0[hour] = -maxDistances0[hour];
				minDistances1[hour] = -maxDistances1[hour];
			}
		}
	}
}



void DriftModelGenerator::computeTrajectoryExtrema(sml::Array4D<TideData>& data, sml::Array4D<TideTrajectory>& trajectories, 
	const size_t iCurrent, size_t iScale, const size_t numBins, const size_t numSamples) const
{
	for (size_t i = 0; i < numBins; i++) {
		std::vector<TideData> tideData(numSamples, { 0.0, nullptr });
		for (size_t iSample = 0; iSample < numSamples; iSample++) {
			std::array<double, 24>& trajectory = trajectories[iCurrent][iScale][i][iSample].distances;
			tideData[iSample].trajectory = &trajectories[iCurrent][iScale][i][iSample].distances;
			for (size_t hour = 0; hour < 24; hour++) {
				tideData[iSample].extremum = trajectory[hour] > tideData[iSample].extremum ? trajectory[hour] : tideData[iSample].extremum;
			}
		}

		// Sort from high to low as only values larger than a given distance need to be evaluated in the DriftModel
		std::sort(tideData.begin(), tideData.end(), [](const TideData& a, const TideData& b) {return a.extremum > b.extremum;});

		// Apply to datastructure
		for (size_t iSample = 0; iSample < numSamples; iSample++) {
			data[iCurrent][iScale][i][iSample] = tideData[iSample];
		}
	}
}



double DriftModelGenerator::computeTideVelocity(double t) const
{
	double velocity = 0.0;
	for (const auto& component : m_tideTable) {
		velocity += component->getAmplitude() * std::cos(component->getAngularFrequency() * t + component->getPhase());
	}
	return velocity;
}



const tdb::WindstrengthTable& DriftModelGenerator::getWindStrengthTable(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& wss) const
{
	const auto& wst = areaModel.getWindstrengthTables();
	std::string wstUID = wss.getWindTableUid();
	const auto& it = wst.find(wstUID);
	if (it == wst.end()) {
		LogError("Inconsistency found in the database, windstrength table with UID " << wstUID << " can not be found.");
	}
	if (wst.size() == 0 || wst.size() > 12) {
		LogError("Wind strength table of weatherstation " <<  wss.getUID() << " has incorrect number of entries. Expecting between one and twelve.");
	}
	return it->second;
}



const tdb::CurrentTable& DriftModelGenerator::getCurrentTable(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& wss) const
{
	const auto& ct = areaModel.getCurrentTables();
	std::string ctUID = wss.getCurrentTableUid();
	const auto& it = ct.find(ctUID);
	if (it == ct.end()) {
		LogError("Inconsistency found in the database, current table with UID " << ctUID << " can not be found.");
	}
	return it->second;
}



const tdb::TideTable& DriftModelGenerator::getTideTable(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& wss) const
{
	const auto& tt = areaModel.getTideTables();
	std::string ttUID = wss.getTideTableUid();
	const auto& it = tt.find(ttUID);
	if (it == tt.end()) {
		LogError("Inconsistency found in the database, tide table with UID " << ttUID << " can not be found.");
	}
	return it->second;
}



const double DriftModelGenerator::getTidePeriod() const
{
	// If there is only one component, take that as the peroid
	if (m_tideTable.size() == 1) {
		if (m_tideTable[0]->getAngularFrequency() == 0.0) {
			return 0.0;
		}
		return 2.0 * std::numbers::pi / m_tideTable[0]->getAngularFrequency();
	}

	// Take the beat period of the two dominant frequencies
	std::vector<std::pair<double, double>> list;
	for (const auto& component : m_tideTable) {
		list.push_back({ component->getAmplitude(), component->getAngularFrequency() });
	}

	// Sort on amplitude
	std::sort(list.begin(), list.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

	// Take the difference between the frequenties
	double angularFrequency = 0.5 * abs(list[1].second - list[0].second);
	if (angularFrequency == 0.0) {
		LogWarning("Incorrect tide data, two duplicate angular frequencies present.");
	}

	return 2.0 * std::numbers::pi / angularFrequency;
}
} // namespace sem

