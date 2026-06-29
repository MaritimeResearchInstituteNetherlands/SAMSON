// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sml/Segment.h>
#include <sio/logger/api.h>
#include <algorithm>
#include <cassert>

namespace sml
{

bool isValidMap(const std::vector<double>& distanceMap)
{
	size_t counter = 0;
	for (const double& distance : distanceMap) {
		if (distance < DBL_MAX) {
			counter++;
		}
	}
	if (counter <= 1) {
		return false;
	}
	return true;
}

// TODO this can be cleaned up in future after shielding has been properly implemented
std::vector<Segment> computeSegments(const sml::Line& line, const double angle, const std::vector<sml::Vector2d>& vertices)
{
	return projectPolygon(line, angle, vertices);
}



std::vector<Segment> projectPolygon(const sml::Line& line, const double angle, const std::vector<sml::Vector2d>& vertices)
{
	std::vector<const std::vector<sml::Vector2d>*> objectVertices(1, &vertices);
	std::vector<std::vector<Segment>> result = projectPolygons(line, angle, objectVertices);
	if (result.size() > 0) {
		return result[0];
	}
	else {
		return std::vector<Segment>();
	}
	return projectPolygons(line, angle, objectVertices)[0];
}



std::vector<std::vector<Segment>> projectPolygons(const sml::Line& line, const double angle, const std::vector<const std::vector<sml::Vector2d>*>& objectVertices)
{
	size_t numObjects = objectVertices.size();
	std::vector<std::vector<Segment>> finalObjectSegments(numObjects);

	// Projection direction
	sml::Vector2d dir(std::cos(angle), std::sin(angle));

	// If the line direction and the angle are similar, the degenerate case must be computed
	double diffAngle = std::acos(dir * line.getDir());
	if ((dir - line.getDir()).norm() < 1e-6) {
		for (size_t i = 0; i < numObjects; i++) {
			finalObjectSegments[i] = computeDegenerateSegment(line, *objectVertices[i]);
		}
		return finalObjectSegments;
	}

	// Generate a discretisation on the line in local coordinates (t \in [0,length])
	sml::Vector2d result;
	std::vector<double> discretisation;
	discretisation.push_back(0.0);
	discretisation.push_back(line.getLength());
	for (size_t iObject = 0; iObject < numObjects; iObject++) {
		const auto& vertices = *objectVertices[iObject];
		for (size_t i = 0; i < vertices.size(); i++) {
			if (line.computeIntersection(vertices[i], dir, result)) {
				if (result[0] > 0 && result[0] < line.getLength()) {
					discretisation.push_back(result[0]);
				}
			}
		}
	}
	
	// Make sure that the discretisation has unique points
	std::sort(discretisation.begin(), discretisation.end());
	auto it = unique(discretisation.begin(), discretisation.end());
	discretisation.erase(it, discretisation.end());

	// Project these coordinates back to the object to generate a closest map of segments
	std::vector<std::vector<double>> objectDistanceMap(numObjects, std::vector<double>(discretisation.size(), DBL_MAX));
	for (size_t iObject = 0; iObject < numObjects; iObject++) {
		const auto& vertices = *objectVertices[iObject];
		for (size_t iPoint = 0; iPoint < discretisation.size(); iPoint++) {
			sml::Vector2d position = line.getStart() + line.getDir() * discretisation[iPoint];
			sml::Vector2d intersectionPoint;
			double closestDistance = DBL_MAX;
			for (size_t i = 0; i < vertices.size() - 1; i++) {
				sml::Line edge(vertices[i], vertices[i + 1]);
				if (edge.projectPointOnLine(position, dir, intersectionPoint, 1e-8)) { // but that is not very robust is it
					closestDistance = std::min(closestDistance, (intersectionPoint - position).norm());
				}
			}
			objectDistanceMap[iObject][iPoint] = closestDistance;
		}
	}

	// Create segments
	std::vector<std::vector<Segment>> objectSegments(numObjects);
	for (size_t iObject = 0; iObject < numObjects; iObject++) {
		const auto& distanceMap = objectDistanceMap[iObject];
		assert(isValidMap(distanceMap));
		auto& segments = objectSegments[iObject];
		for (size_t iPoint = 0; iPoint < discretisation.size() - 1; iPoint++) {
			if (distanceMap[iPoint] < DBL_MAX && distanceMap[iPoint + 1] < DBL_MAX) {
				sml::Vector2d start = line.getStart() + line.getDir() * discretisation[iPoint];
				sml::Vector2d end = line.getStart() + line.getDir() * discretisation[iPoint + 1];
				Segment segment;
				segment.line = Line(start, end);
				segment.distanceStart = distanceMap[iPoint];
				segment.distanceEnd = distanceMap[iPoint + 1];

				// Filter out unreasonable small segments
				if (segment.line.getLength() > 1e-8) {
					segments.push_back(segment);
				}
			}
		}
	}
	return objectSegments;
}



std::vector<Segment> computeDegenerateSegment(const sml::Line& line, const std::vector<sml::Vector2d>& vertices)
{
	// The degenerate case results in a single segment the size of the link with a distance towards the nearest point of the object
	std::vector<Segment> segments;
	double closestPoint = DBL_MAX;
	int closestIndex = 0;
	for (size_t i = 0; i < vertices.size() - 1; i++) {
		Line edge = sml::Line(vertices[i], vertices[i + 1]);
		sml::Vector2d result;
		if (edge.projectPointOnLine(line.getEnd(), line.getDir(), result)) {
			double distance = (result - line.getEnd()).norm();
			if (distance < closestPoint) {
				closestPoint = distance;
				closestIndex = i;
			}
		}
	}

	// Create the segment if applicable
	if (closestPoint < DBL_MAX) {
		Segment segment;
		segment.line = line;
		segment.distanceEnd = closestPoint;
		segment.distanceStart = closestPoint + line.getLength();
		segments.push_back(segment);
	}
	return segments;
}



std::vector<Segment> filterSegments(const std::vector<sml::Segment>& segments)
{
	// Find priority of overlapping segments
	const size_t numSegments = segments.size();
	size_t numActiveSegments = numSegments;			// Assume first that all segments are active
	std::vector<int> active(numSegments, 1);
	for (size_t i = 0; i < numSegments; i++) {
		if (active[i] == 1) {
			const auto& segment_i = segments[i];
			for (size_t j = i + 1; j < numSegments; j++) {
				if (active[j] == 1) {
					const auto& segment_j = segments[j];
					// Check if the segments overlap
					double distanceStart_j;
					double distanceEnd_j;
					if (segment_i.line.getStart() == segment_j.line.getStart() && segment_i.line.getEnd() == segment_j.line.getEnd()) {
						distanceStart_j = segment_j.distanceStart;
						distanceEnd_j = segment_j.distanceEnd;
					}
					else if (segment_i.line.getStart() == segment_j.line.getEnd() && segment_i.line.getEnd() == segment_j.line.getStart()) {
						// Vertices are swaped, flip the distance
						distanceStart_j = segment_j.distanceEnd;
						distanceEnd_j = segment_j.distanceStart;
					}
					else {
						// Segments do not overlap
						continue;
					}

					// Check which segment has priority
					if (segment_i.distanceStart <= distanceStart_j && segment_i.distanceEnd <= distanceEnd_j) {
						// Disable segment_j
						active[j] = 0;
						numActiveSegments--;
					}
					else if (segment_i.distanceStart >= distanceStart_j && segment_i.distanceEnd >= distanceEnd_j) {
						// Disable current segment_i and break off the for-loop
						active[i] = 0;
						numActiveSegments--;
						break;
					}
					else {
						// TODO ensure that the input polygon does not intersect itself
						LogError("Unexpected case during segment computation. Polygon might intersect itself.");
					}
				}
			}
		}
	}

	// Collect active segments
	std::vector<Segment> finalSegments(numActiveSegments);
	size_t localIndex = 0;
	for (size_t i = 0; i < numSegments; i++) {
		if (active[i]) {
			//finalSegments.push_back(segments[i]);
			finalSegments[localIndex] = segments[i];
			localIndex++;
		}
	}
	return finalSegments;
}



std::vector<std::vector<Segment>> filterSegments(const std::vector<std::vector<sml::Segment>>& objectSegments, std::vector<int>& mask)
{
	size_t numObjects = objectSegments.size();

	// Assume all segments are active
	std::vector<std::vector<int>> active(numObjects);
	for (size_t i = 0; i < numObjects; i++) {
		active[i].resize(objectSegments[i].size(), 1);
	}

	// Find priority of overlapping segments
	for (size_t i = 0; i < numObjects; i++) {
		if (mask[i] == 1) {	// Exclude masked objects
			for (size_t si = 0; si < objectSegments[i].size(); si++) {
				if (active[i][si] == 1) {
					const auto& segment_i = objectSegments[i][si];
					for (size_t j = 0; j < numObjects; j++) {
						bool continueWithNextSegment = false;
						if (mask[j] == 1) {
							for (size_t sj = 0; sj < objectSegments[j].size(); sj++) {
								const auto& segment_j = objectSegments[j][sj];

								// Check if the segments overlap
								double distanceStart_j;
								double distanceEnd_j;
								if (segment_i.line.getStart() == segment_j.line.getStart() && segment_i.line.getEnd() == segment_j.line.getEnd()) {
									distanceStart_j = segment_j.distanceStart;
									distanceEnd_j = segment_j.distanceEnd;
								}
								else if (segment_i.line.getStart() == segment_j.line.getEnd() && segment_i.line.getEnd() == segment_j.line.getStart()) {
									// Vertices are swaped, flip the distance
									distanceStart_j = segment_j.distanceEnd;
									distanceEnd_j = segment_j.distanceStart;
								}
								else {
									// Segments do not overlap
									continue;
								}

								// Check which segment has priority
								if (segment_i.distanceStart <= distanceStart_j && segment_i.distanceEnd <= distanceEnd_j) {
									// Disable segment_j
									active[j][sj] = 0;
								}
								else if (segment_i.distanceStart >= distanceStart_j && segment_i.distanceEnd >= distanceEnd_j) {
									// Disable current segment_i and break off the for-loop
									active[i][si] = 0;
									continueWithNextSegment = true;
									break;
								}
								else {
									// TODO ensure that the input polygon does not intersect itself
									LogError("Unexpected case during segment computation. Polygon might intersect itself.");
								}
								
							}
						}

						// Break off the for-loop if the current segment_i is inactive;
						if (continueWithNextSegment) {
							break;
						}
					}
				}
			}
		}
	}



	// Collect active segments
	std::vector<std::vector<Segment>> finalSegments(numObjects);
	for (size_t i = 0; i < numObjects; i++) {
		if (mask[i] == 1) {
			const auto& objectSegments_i = objectSegments[i];
			auto& finalSegments_i = finalSegments[i];
			for (size_t j = 0; j < objectSegments_i.size(); j++) {
				if (active[i][j] == 1) {
					finalSegments_i.push_back(objectSegments_i[j]);
				}
			}

		}
	}
	return finalSegments;
}

} // namespace sml
