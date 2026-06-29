// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <tdb/AreaModel.h>
#include <tdb/TrafficDataBase.h>
#include <sml/AngleBounds.h>
#include <sml/AngularDistributionFunction.h>
#include <sml/Array4d.h>
#include <sml/Array5d.h>
#include <sem/DriftModel.h>
#include <array>
#include <vector>

namespace sem
{

class DriftModelGenerator
{
public:
	DriftModelGenerator(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& weatherStation, bool useEngineRepair, bool useAnchoring);

	void compute(DriftModel& driftModel, const std::string uid, const double draft, const double height) const;

private:
	double computeDragVelocity(size_t scale, double zeta, double elevationMin, double elevationMax) const;

	double computeTideVelocity(double t) const;

	sml::AngleBounds computeDriftBounds(const double r, const double a0lim, const double angle) const;

	std::vector<double> computeDriftAngles(const sml::AngleBounds& driftBounds) const;

	void computeTrajectories(DriftModel& driftModel, const std::vector<double>& driftAngles,
		const std::vector<double>& ve0, const std::vector<double>& ve1, const size_t iCurrent, const size_t iScale) const;

	void computeTrajectoryExtrema(sml::Array4D<TideData>& data, sml::Array4D<TideTrajectory>& trajectories, 
		const size_t iCurrent, const size_t iScale, const size_t numBins, const size_t numSamples) const;

	const tdb::WindstrengthTable& getWindStrengthTable(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& wss) const;

	const tdb::CurrentTable& getCurrentTable(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& wss) const;

	const tdb::TideTable& getTideTable(const tdb::AreaModel& areaModel, const tdb::WeatherstationScheme& wss) const;

	const double getTidePeriod() const;

	// Database input data
	const tdb::WeatherstationScheme& m_weatherstationScheme;
	const tdb::CurrentTable& m_currentTable;
	const tdb::TideTable& m_tideTable;
	const tdb::WindstrengthTable& m_windStrengthTable;
	bool m_useEngineRepair;
	bool m_useAnchoring;

	// Processed data
	std::array<sml::AngularDistributionFunction, 13> m_pWindDirections;		// Per scale an angular distribution function for the wind direction
	std::vector<std::array<double, 24>> m_tideVelocities;					// Per num samples the velocity at each simulation hour
	const size_t m_numTideSamples;
	const double m_tidePeriod;
	const double m_tideAngle;
};

} // namespace sem

