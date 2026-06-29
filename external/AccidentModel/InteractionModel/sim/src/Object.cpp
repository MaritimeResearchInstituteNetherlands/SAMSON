// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/Object.h>
#include <sml/Polygon.h>
#include <sml/GeoUtils.h>

namespace sim
{
Object::Object(const size_t objectIndex, const tdb::Data<tdb::ObjectScheme>& objectScheme, const std::vector<sml::Coordinate2D>& coordinates)
	: ModelEntity(objectIndex)
	, m_objectScheme(objectScheme)
	, m_isClosed(false)
	, m_area(std::numeric_limits<double>::quiet_NaN())
{
	for (auto& coordinate : coordinates) {
		addCoordinate(coordinate);
	}

	size_t lastCoordinates = coordinates.size() - 1;
	if (coordinates[0].lat == coordinates[lastCoordinates].lat
		&& coordinates[0].lon == coordinates[lastCoordinates].lon) {
		m_isClosed = true;

		// Compute its area
		double maxLat = -DBL_MAX;
		double minLat = DBL_MAX;
		double maxLon = -DBL_MAX;
		double minLon = DBL_MAX;
		for (auto& coordinate : coordinates) {
			maxLat = coordinate.lat > maxLat ? maxLat : coordinate.lat;
			minLat = coordinate.lat < minLat ? minLat : coordinate.lat;
			maxLon = coordinate.lon > maxLon ? maxLat : coordinate.lon;
			minLon = coordinate.lon < minLon ? minLat : coordinate.lon;
		}
		sml::Coordinate2D origin{ 0.5 * (maxLat + minLat), 0.5 * (maxLon + minLon) };

		sml::Polygon p;
		for (auto& coord : coordinates) {
			p.addVertex(sml::GeoUtils::projectToPlane(origin, coord));
		}
		m_area = p.getArea();

		LogVerbose("Object " << getUID() << " is closed and has an area of " << m_area);
	}

}
} // namespace sim
