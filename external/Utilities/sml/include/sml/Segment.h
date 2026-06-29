// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sml_export.h>
#include <sml/Vector.h>
#include <sml/Line.h>
#include <vector>

namespace sml
{




struct SML_EXPORT Segment
{
	std::string tag;		// Tag to give the segment some semantics
	sml::Line line;			// Projected line on the given link
	double distanceStart;	// Distance from start to object
	double distanceEnd;		// Distance from end to object
};


// Projects a polyline on a line, generating a set of non-overlapping segments
std::vector<Segment> SML_EXPORT computeSegments(const sml::Line& line, const double angle, const std::vector<sml::Vector2d>& vertices);

// Projects a polygon onto a line and creates a set of overlapping segments
std::vector<Segment> SML_EXPORT projectPolygon(const sml::Line& line, const double angle, const std::vector<sml::Vector2d>& vertices);

// Projects a set of polygon onto a line and creates a set of overlapping segments
std::vector<std::vector<Segment>> SML_EXPORT projectPolygons(const sml::Line& line, const double angle, const std::vector<const std::vector<sml::Vector2d>*>& vertices);

// Creates a segment in the degenerate case where the angle is in the same direction as the line.
std::vector<Segment> computeDegenerateSegment(const sml::Line& line, const std::vector<sml::Vector2d>& vertices);

// Filters a set of overlapping segments into a set of non-overlapping segments
std::vector<Segment> SML_EXPORT filterSegments(const std::vector<sml::Segment>& segments);

// Filters a segments of a set of objects into a total set of non-overlapping segments, the mask flags if the object is included(1) or excluded(0)
std::vector<std::vector<Segment>> SML_EXPORT filterSegments(const std::vector<std::vector<sml::Segment>>& segments, std::vector<int>& mask);

} // namespace sml
