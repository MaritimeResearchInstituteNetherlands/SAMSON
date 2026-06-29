// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
#pragma once

#include <tuple>
#include <sem/ExposureModel.h>
#include <sml/Vector.h>

namespace sem {
class CellObject;
class CellMeetingData;

class SubCellObject
{
	struct Projection
	{
		sml::AngleBounds bin;						// Bin angles of the projection w.r.t. the global system
		std::vector<double> objectDistances;		// Segments of the projection gathered per object
		std::vector<size_t> objectIndices;			// Connects the objects to the segments
		std::vector<size_t> localObjectIndices;		// Local to the projection computation
	};

public:
	SubCellObject(const sml::Vector2d& position, double area, const CellObject& cellObject);

	void setup();

	void cleanUp();

	void computeExposuresDrift(const double elevationMin, const double elevationMax, const DriftModel& driftModel, CellMeetingData& meetingData);

	void computeExposuresRam(const double elevationMin, const double elevationMax, const double direction, CellMeetingData& meetingData);

	void computeCollisionsDrift(const double elevationMin, const double elevationMax, const DriftModel& driftModel, CellMeetingData& meetingData);

	void computeCollisionsRam(const double elevationMin, const double elevationMax, const double direction, const double length, CellMeetingData& meetingData);

private:
	void computeProjections();

	double computeRamProbability(const double direction, Projection projection);

	int getDistanceIndex(const std::vector<size_t>& objectIndices, const std::vector<double>& objectDistances, const double elevationMin, const double elevationMax) const;

	bool isRamProjection(const double targetDistance, const double direction, const Projection& projection);

	const CellObject& m_cellObject;				// Parent LinkObject class
	const sml::Vector2d m_position;				// Position of the cell centre
	const double m_area;						// Area of the cell
	std::vector<sml::AngleBounds> m_bounds;		// Bounds of non-shielding objects w.r.t this sub link
	std::vector<size_t> m_numBins;				// Number of bins the non shielding bound is divided in
	std::vector<Projection> m_projections;		// Projections of the discretised object bounds
	size_t m_totalBins;							// Used for faster memory allocation during setup
};

} // namespace sem

