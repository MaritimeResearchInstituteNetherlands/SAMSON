// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/LinkObject.h>
#include <sim/Link.h>
#include <sim/Object.h>
#include <sml/GeoUtils.h>

namespace sim
{
LinkObject::LinkObject(const Link& link)
	: m_link(link)
	, m_origin(computeOrigin(link.getCoordinates()[0], link.getCoordinates()[1]))
	, m_p0(sml::GeoUtils::projectToPlane(m_origin, link.getCoordinates()[0]))
	, m_p1(sml::GeoUtils::projectToPlane(m_origin, link.getCoordinates()[1]))
{
}



sml::Coordinate2D LinkObject::computeOrigin(const sml::Coordinate2D& c0, const sml::Coordinate2D& c1)
{
	sml::Coordinate2D origin;
	origin.lat = c0.lat + 0.5 * (c1.lat - c0.lat);
	origin.lon = c0.lon + 0.5 * (c1.lon - c0.lon);

	return origin;
}
} // namespace sim
