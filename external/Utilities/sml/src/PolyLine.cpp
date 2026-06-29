// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
#include <sml/PolyLine.h>
#include <sio/logger/api.h>
#include <algorithm>
#include <cassert>

namespace sml
{

// Returns true if something has been clipped
bool PolylineClipper::clip(const Vertices& polyLine, const sml::Line& line, std::vector<Vertices>& newPolyLines)
{
	if (polyLine.size() == 0) {
		return false;
	}

	// Initialise the first part
	newPolyLines.clear();
	newPolyLines.push_back(Vertices());
	newPolyLines.back().push_back(polyLine[0]);
	sml::Vector2d result;
	sml::Vector2d dir(-line.getDir()[1], line.getDir()[0]); // Orthogonal direction to the line
	line.computeIntersection(polyLine[0], dir, result);
	bool inside = result[1] < 0;
	size_t numInside = 0;
	// Cut the polyline in pieces
	for (size_t i = 1; i < polyLine.size(); i++) {
		line.computeIntersection(polyLine[i], dir, result);
		if (inside != result[1] < 0) {
			// Compute intersection
			sml::Vector2d dir = (polyLine[i] - newPolyLines.back().back());
			dir.normalise();
			if (!line.computeIntersection(newPolyLines.back().back(), dir, result)) {
				LogError("Internal error. The intersection between a polyline and a line were incorrectly computed.");
				newPolyLines.clear();
				return false;
			}

			// Close and create new part
			if (result[0] > 0 && result[0] < line.getLength()) {
				sml::Vector2d intersection = line.getStart() + line.getDir() * result[0];

				// Close part
				newPolyLines.back().push_back(intersection - dir * 1e-8); // Add a small offset to ensure the path truly lies within inside/outside
				if (inside) numInside++;

				// Create new part
				inside = !inside;		// Flip to otherside
				newPolyLines.push_back(Vertices({ intersection + dir * 1e-8 }));
			}
		}

		// Add vertex
		newPolyLines.back().push_back(polyLine[i]);
	}

	// Return true if clipping is required
	return newPolyLines.size() > 1;
} 
} // namespace sml 
