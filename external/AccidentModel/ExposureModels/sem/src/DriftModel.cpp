// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
#include <sem/DriftModel.h>
#include <sem/DataModel.h>
#include <numbers>
#include <cmath>
#include <cassert>

namespace sem
{

DriftModel::DriftModel()
	: m_numCurrents(0)
	, m_maxNumDirections(0)
	, m_numTideSamples(0)
	, m_useEngineRepair(true)
	, m_useAnchoring(true)
{
}



void DriftModel::resize(size_t numCurrents, size_t maxNumDirections, size_t numSamples)
{
	if (numCurrents == 0 || maxNumDirections > m_maxNumDirections) {
		m_numCurrents = numCurrents;
		m_maxNumDirections = maxNumDirections;
		m_numTideSamples = numSamples;
		m_r.resize(numCurrents);
		m_pCurrent.resize(numCurrents);
		m_maxTrajectories0.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_maxTrajectories1.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_minTrajectories0.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_minTrajectories1.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_minTideData0.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_maxTideData0.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_minTideData1.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_maxTideData1.resize(numCurrents, 13, maxNumDirections, numSamples);
		m_pDriftAngle0.resize(numCurrents);
		m_pDriftAngle1.resize(numCurrents);
	}
}


// The probability to drift in the direction of bounds is given by integrating the drift direction probability times the probability of the tide hitting the object
double DriftModel::computeExposureProbability(const sml::AngleBounds& bounds, const double distance) const
{
	double oppositeLowerBound = sml::normalisedPositiveAngles(bounds.getLowerBound() + std::numbers::pi);
	double oppositeUpperBound = sml::normalisedPositiveAngles(bounds.getUpperBound() + std::numbers::pi);

	// Function that evaluates the hit chance for a given drift direction index
	TideData const* const* tideLimits;	// Pointer to the relevant extremum distance data as function of driftIndex and tideIndex
	double factor = 1.0 / m_numTideSamples;
	auto hitProbability = [&](size_t driftIndex) {
		auto& data = tideLimits[driftIndex];

		// Base method, simple counting
		//size_t numHit_ = 0;
		//for (size_t i = 0; i < m_numTideSamples; i++) {
		//	if (data[i].extremum > distance) {
		//		numHit_++;
		//	}
		//}

		// Method 0
		//auto lower = std::lower_bound(data.begin(), data.end(), distance, [](const TideData& a, const double distance) {return a.extremum >= distance; });
		//return std::distance(data.begin(), lower) * factor;
		// Fast if items  > 200

		// Method 1
		//size_t numHit = 10;
		for (size_t i = 0; i < m_numTideSamples; i++) {
			if (data[i].extremum < distance) {
				//numHit = i;
				//break;
				return double(i) * factor;
			}
		};
		return 1.0;

		// Method 2
		//auto* ptr = data;
		//auto* end = ptr + m_numTideSamples;
		////size_t numHit = m_numTideSamples;
		//for (auto* p = ptr; p != end; p++) {
		//	if (p->extremum < distance) {
		//		//numHit = p - ptr;
		//		//break;
		//		return double(p - ptr) * factor;
		//	}
		//}
		//return 1.0;
		
		//if ((numHit - numHit_) != 0) {
		//	std::cout << "Something went wrong!" << std::endl;
		//	std::cout << "numHit=" << numHit << ", numHit_=" << numHit_ << std::endl;
		//	std::cout << "Distance=" << distance << std::endl;
		//	for (size_t i = 0; i < m_numTideSamples; i++) {
		//		std::cout << data[i].extremum << std::endl;
		//	}
		//	std::exit(-1);
		//}

		//return double(numHit) * factor;
		//Method 2
		//auto* ptr = data.data();
		//auto* end = ptr + m_numTideSamples;
		//
		//for (auto* p = ptr; p != end; ++p) {
		//	if (p->extremum < distance) {
		//		return static_cast<double>(p - ptr) * factor;
		//	}
		//}
		// TODO Check if I did this function correct
		};

	double totalProbability = 0.0;
	for (size_t iCurrent = 0; iCurrent < m_numCurrents; iCurrent++) {
		// Cache pointers for given current
		const auto& maxTideData0_iCurrent = m_maxTideData0[iCurrent];
		const auto& minTideData0_iCurrent = m_minTideData0[iCurrent];
		const auto& maxTideData1_iCurrent = m_maxTideData1[iCurrent];
		const auto& minTideData1_iCurrent = m_minTideData1[iCurrent];
		const auto& pDriftAngle0_iCurrent = m_pDriftAngle0[iCurrent];
		const auto& pDriftAngle1_iCurrent = m_pDriftAngle1[iCurrent];
		const auto& r_iCurrent = m_r[iCurrent];

		double currentProbability = 0.0;
		for (size_t iScale = 0; iScale < m_numScales; iScale++) {
			double pHit = 0.0;	// Chance of hitting the object in a certain direction for the given current and windspeed
			
			// First solution in the forward direction
			tideLimits = maxTideData0_iCurrent[iScale];
			pHit += pDriftAngle0_iCurrent[iScale].integrate(bounds.getLowerBound(), bounds.getUpperBound(), hitProbability);

			// First solution in the backward direction
			tideLimits = minTideData0_iCurrent[iScale];
			pHit += pDriftAngle0_iCurrent[iScale].integrate(oppositeLowerBound, oppositeUpperBound, hitProbability);

			// Second solution if r > 1
			if (r_iCurrent[iScale] > 1) {
				// Forward direction
				tideLimits = maxTideData1_iCurrent[iScale];
				pHit += pDriftAngle1_iCurrent[iScale].integrate(bounds.getLowerBound(), bounds.getUpperBound(), hitProbability);

				// Backward dircetion
				tideLimits = minTideData1_iCurrent[iScale];
				pHit += pDriftAngle1_iCurrent[iScale].integrate(oppositeLowerBound, oppositeUpperBound, hitProbability);
			}
			// Add the wind probability
			assert(pHit <= 1.0);
			currentProbability += pHit * m_pScale[iScale];
		}

		// Add the current probability
		assert(currentProbability <= 1.0);
		totalProbability += currentProbability * m_pCurrent[iCurrent];
		
	}
	assert(totalProbability <= 1.0);
	return totalProbability;
}



double DriftModel::computeMeasureProbability(const sml::AngleBounds& bounds, const double distance) const
{
	double oppositeLowerBound = sml::normalisedPositiveAngles(bounds.getLowerBound() + std::numbers::pi);
	double oppositeUpperBound = sml::normalisedPositiveAngles(bounds.getUpperBound() + std::numbers::pi);

	double totalProbability = 0.0;
	for (size_t iCurrent = 0; iCurrent < m_numCurrents; iCurrent++) {
		//std::cout << "iCurrent=" << iCurrent << std::endl;
		double currentProbability = 0.0;
		// Cache pointers for given current
		const auto& maxTideData0_iCurrent = m_maxTideData0[iCurrent];
		const auto& minTideData0_iCurrent = m_minTideData0[iCurrent];
		const auto& maxTideData1_iCurrent = m_maxTideData0[iCurrent];
		const auto& minTideData1_iCurrent = m_minTideData0[iCurrent];
		const auto& pDriftAngle0_iCurrent = m_pDriftAngle0[iCurrent];
		const auto& pDriftAngle1_iCurrent = m_pDriftAngle1[iCurrent];
		const auto& r_iCurrent = m_r[iCurrent];
		for (size_t iScale = 0; iScale < m_numScales; iScale++) {
			size_t scale = m_scale[iScale];
			//std::cout << "scale=" << scale << std::endl;
			double pHitTotal = 0.0;	// Chance of hitting the object in a certain direction for the given current and windspeed

			// Function that evaluates the hit chance for a given drift direction index
			TideData const* const* tideLimits;	// Pointer to the relevant distance data as function of driftIndex and tideIndex
			double factor = 1.0 / m_numTideSamples;
			auto hitProbability = [&](size_t driftIndex) {
				double pHit = 0;
				TideData const* data = tideLimits[driftIndex];
				for (size_t iSample = 0; iSample < m_numTideSamples; iSample++) {
					//std::cout << "iSample=" << iSample << std::endl;
					const TideData& sample = data[iSample];

					if (sample.extremum <= distance) {
						break; // Rest of the samples have a too small distance
					}
					const std::array<double, 24>& trajectory = *sample.trajectory;
					for (size_t hourIndex = 1; hourIndex < 24; hourIndex++) {
						//std::cout << "hourIndex=" << hourIndex << std::endl;
						if (trajectory[hourIndex] > distance) {
							// Time to collision = t_hour + t_local
							// distance = D_{h-1} + v_local t_local
							// v_local =  (D_{h} -  D_{h-1}) / 3600
							size_t hour_prev = hourIndex - 1;
							double position_prev = trajectory[hour_prev];
							double dx_inv = 1.0 / (trajectory[hourIndex] - position_prev);
							double t = ((distance - position_prev) * dx_inv + hour_prev) * 3600;

							// Apply collision probability including measures
							double pMeasures = 1;

							// Engine repair function
							if (m_useEngineRepair) {
								constexpr double g0 = 3600 / 1.5;
								constexpr double g1 = g0 - 900;
								pMeasures *= t < 900 ? 1 : g0 / (t + g1);
							}

							pHit += pMeasures;
							break;
						}
					}
				}	
				return pHit * factor;
				};

			

			// First solution in the forward direction
			tideLimits = maxTideData0_iCurrent[iScale];
			pHitTotal += pDriftAngle0_iCurrent[iScale].integrate(bounds.getLowerBound(), bounds.getUpperBound(), hitProbability);

			// First solution in the backward direction
			tideLimits = minTideData0_iCurrent[iScale];
			pHitTotal += pDriftAngle0_iCurrent[iScale].integrate(oppositeLowerBound, oppositeUpperBound, hitProbability);

			// Second solution if r > 1
			if (r_iCurrent[iScale] > 1) {

				// Forward direction
				tideLimits = maxTideData1_iCurrent[iScale];
				pHitTotal += pDriftAngle1_iCurrent[iScale].integrate(bounds.getLowerBound(), bounds.getUpperBound(), hitProbability);

				// Backward dircetion
				tideLimits = minTideData1_iCurrent[iScale];
				pHitTotal += pDriftAngle1_iCurrent[iScale].integrate(oppositeLowerBound, oppositeUpperBound, hitProbability);
			}
			// Add the wind probability
			assert(pHitTotal <= 1.0);
			if (m_useAnchoring) {
				pHitTotal *= GeneralModelData::get().getAnchorFailure(scale);
			}
			currentProbability += pHitTotal * m_pScale[iScale];
		}

		// Add the current probability
		assert(currentProbability <= 1.0);
		totalProbability += currentProbability * m_pCurrent[iCurrent];
	}
	assert(totalProbability <= 1.0);
	return totalProbability;
}

} // namespace sem

