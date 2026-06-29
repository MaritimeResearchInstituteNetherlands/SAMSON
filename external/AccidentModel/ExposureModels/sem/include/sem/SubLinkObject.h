// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sem/ExposureModel.h>
#include <sem/DriftModel.h>
#include <sml/Vector.h>
#include <sml/Coordinate.h>
#include <sml/AngleBounds.h>
#include <sml/Segment.h>
#include <sim/Object.h>
#include <sim/LinkObject.h>
#include <vector>
#include <deque>

namespace sem
{
class LinkObject;
class LinkObjectMeetingData;

class SubLinkObject
{
	using ObjectSegments = std::vector<std::vector<sml::Segment>>;	// Segments sorted per object

	struct ProjectionDrift
	{
		sml::AngleBounds bin;						// Bin angles of the projection w.r.t. the global system
		ObjectSegments objectSegments;				// Segments of the projection gathered per object
		std::vector<size_t> localObjectIndices;		// Local to the projection computation
	};

	struct ProjectionRam
	{
		double probability;							// Probability of ram happening, independent of ship properties
		ObjectSegments objectSegments;				// Segments of the projection gathered per object
		std::vector<size_t> localObjectIndices;		// Local to the projection computation
	};

public:
	SubLinkObject(const sml::Vector2d p0, const sml::Vector2d p1, const double offset, const LinkObject& linkObject);

	void setup();

	void cleanup();

	inline const double getOffset() const { return m_offset; }

	inline size_t getNumProjectionsDrift() const { return m_projectionsDrift.size(); }

	inline size_t getNumProjectionsRam() const { return m_projectionsRam.size(); }

	void computeExposuresDrift(const double pSubLink, const double elevationMin, const double elevationMax, const DriftModel& driftModel, LinkObjectMeetingData& meeting_i);

	void computeExposuresRam(const double pSubLink, const double elevationMin, const double elevationMax, LinkObjectMeetingData& meeting_i);

	void computeCollisionsDrift(const double pSubLink, const double elevationMin, const double elevationMax, const DriftModel& driftModel, LinkObjectMeetingData& meeting);

	void computeCollisionsRam(const double pSubLink, const double elevationMin, const double elevationMax, const double length, LinkObjectMeetingData& meeting);
private:
	void gatherObjects();

	void computeProjections();

	void computeDriftProjections();

	void computeRamProjections();

	bool isInRamArea(const std::vector<sml::Vector2d>& vertices, const sml::AngleBounds& localRamBound, const sml::AngleBounds& binBounds);

	std::vector<sml::AngleBounds> getBins(const sml::AngleBounds& bounds, const std::vector<double>& discretisation);

	void createBins(std::vector<sml::AngleBounds>& bins, const double lowerBound, const double upperBound, const std::vector<double>& discretisation);

	void getValidSegments(ObjectSegments& validSegments, const std::vector<size_t>& objectIndices, const ObjectSegments& objectSegments, const double elevationMin, const double elevationMax);

	// Description of the sub link
	const LinkObject& m_linkObject;								// Parent LinkObject class
	const sml::Line m_line;										// Mathematical description of the sub-link
	const double m_offset;										// Offset of the sub-link w.r.t the link

	// The bounds of objects interacting with the sub link
	std::deque<std::vector<sml::Vector2d>> m_localVertices;		// Contains the vertices for each local object
	std::vector<int> m_objectIndices;							// References to objects that interact witht his sublink (intersecting objects occur more than once)
	std::vector<const std::vector<sml::Vector2d>*> m_vertices;	// References to vertices of the corresponding objects
	std::vector<sml::AngleBounds> m_bounds;						// Bounds of non-shielding objects w.r.t this sub link
	//std::vector<sml::AngleBounds> m_boundsShielded;			// Bounds of shielding objects w.r.t this sub link

	// Contains all the projections of objects onto the sub-link which have to be evaluated by a ship
	std::vector<ProjectionDrift> m_projectionsDrift;
	std::vector<ProjectionRam> m_projectionsRam;
};
} // namespace sem

