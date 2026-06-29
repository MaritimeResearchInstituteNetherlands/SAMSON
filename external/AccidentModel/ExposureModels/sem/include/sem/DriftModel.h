// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once
#include <sml/AngleBounds.h>
#include <sml/AngularDistributionFunction.h>
#include <sml/Array3d.h>
#include <sml/Array4d.h>
#include <sml/Array5d.h>
#include <array>
#include <vector>

namespace sem
{

struct TideTrajectory
{
	std::array<double, 24> distances{}; // The distance per hour
};

struct TideData
{
	double extremum = 0;								// Either the maximum or the absolute minimum of a trajectory
	std::array<double, 24>* trajectory = nullptr;		// Pointer to the trajectory
};



class DriftModel
{
public:
	DriftModel();

	// Computes the probability that a ship drifts in the direction of the bounds and hits a target at the supplied distance
	double computeExposureProbability(const sml::AngleBounds& bounds, const double distance) const;

	double computeMeasureProbability(const sml::AngleBounds& bounds, const double distance) const;

	inline std::string getShipCategoryUID() const { return m_shipCategoryUid; }

	inline std::string getWeatherStationUID() const { return m_weatherStationUid; }

	inline const std::vector<double>& getDiscretisation() const { return m_discretisation; }

private:
	using DriftAngleDistributions = std::vector<std::array<sml::AngularDistributionFunction, 13>>;

	void resize(size_t numCurrents, size_t numDirections, size_t numSamples);

	// TODO test the performance of the std::vector<std::array>> approach over the sml::Array2D
	// Drift model is a function of shipclass and weather station
	std::string m_shipCategoryUid;				// Tag to keep track to which ship category this data belongs to
	std::string m_weatherStationUid;			// Tag to keep track to which weatherstation this data belongs to

	sml::Array4D<TideTrajectory> m_maxTrajectories0;
	sml::Array4D<TideTrajectory> m_maxTrajectories1;
	sml::Array4D<TideTrajectory> m_minTrajectories0;	// mirror trajectories (minus sign) perhaps change this at some point to reduce memory load
	sml::Array4D<TideTrajectory> m_minTrajectories1;
	sml::Array4D<TideData> m_minTideData0;
	sml::Array4D<TideData> m_maxTideData0;
	sml::Array4D<TideData> m_minTideData1;
	sml::Array4D<TideData> m_maxTideData1;

	// Drift angular distribution function without tide
	DriftAngleDistributions m_pDriftAngle0;		// First part of the drift angle probability function per current and wind scale
	DriftAngleDistributions m_pDriftAngle1;		// Second part of the drift angle probability function per current and wind scale
	std::vector<std::array<double,13>> m_r;		// Ratio of current velocity and drag velocity
	std::vector<double> m_pCurrent;				// Stores the current probability
	std::array<double, 13> m_scale;				// Stores the actual scale of the entry
	std::array<double, 13> m_pScale;			// Stores the probability on a given scale

	// Number of components in the data
	size_t m_numCurrents;						// Number of currents
	size_t m_numScales;							// Number of scales to be evaluated
	size_t m_maxNumDirections;					// The maximum number of drift angles
	size_t m_numTideSamples;					// Number of tide samples
	std::vector<double> m_discretisation;		// All evaluated angles

	// Measures
	bool m_useEngineRepair;
	bool m_useAnchoring;

	friend class DriftModelGenerator;			// DriftModelGenerator is the only class that is allowed to fill in this data
};
} // namespace sem

