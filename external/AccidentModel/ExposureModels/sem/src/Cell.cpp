// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/Cell.h>
#include <sml/Line.h>
#include <sml/Polygon.h>
#include <sml/GeoUtils.h>

namespace sem
{
Cell::Cell(const size_t& index, const tdb::Data<tdb::CellScheme>& cellScheme, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase)
	: base(index, cellScheme, trafficDatabase.getTrafficModel())
	, m_model(model)
	, m_trafficDatabase(trafficDatabase)
{
}



void Cell::setup()
{
	// Determine cell origin
	const auto& coordinates = getCoordinates();
	double maxLat = -DBL_MAX;
	double minLat = DBL_MAX;
	double maxLon = -DBL_MAX;
	double minLon = DBL_MAX;
	for (const auto& coord : coordinates) {
		maxLat = coord.lat > maxLat ? coord.lat : maxLat;
		minLat = coord.lat < minLat ? coord.lat : minLat;
		maxLon = coord.lon > maxLon ? coord.lon : maxLon;
		minLon = coord.lon < minLon ? coord.lon : minLon;
	}
	m_origin = { 0.5 * (maxLat + minLat), 0.5 * (maxLon + minLon) };

	// Convert from polyline to r.h.s polygon - skip last vertex
	std::vector<sml::Vector2d> points;
	for (size_t i = 0; i < coordinates.size() - 1; i++) { 
		points.push_back(sml::GeoUtils::projectToPlane(m_origin, coordinates[i]));
	}
	sml::Vector2d p10 = (points[1] - points[0]);
	sml::Vector2d p20 = (points[2] - points[0]);
	double n = p10[0] * p20[1] - p10[1] * p20[0];
	if (n < 0) {
		for (auto it = points.rbegin(); it != points.rend(); it++) {
			m_polygon.addVertex(*it);
		}
	}
	else {
		for (auto it = points.begin(); it != points.end(); it++) {
			m_polygon.addVertex(*it);
		}
	}

	m_area = m_polygon.getArea();

	m_zoneScheme = findPilotAssistanceZone(m_trafficDatabase.getAreaModel().getZones(), m_trafficDatabase.getAreaModel().getVertices());

	LogVerbose("Cell " << getUID() << " has an origin of " << m_origin << " and an area of " << m_area);
}



void Cell::computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j)
{
	// Initial values
	exposure = 0.0;
	struckChance_i = 0.0;
	struckChance_j = 0.0;

	// Grab the meeting data
	const auto& shipCellData_i = getShipCellData(meeting.i);
	const auto& shipCellData_j = getShipCellData(meeting.j);
	const auto& shipCategory_i = getShipCategory(meeting.i);
	const auto& shipCategory_j = getShipCategory(meeting.j);

	const double u_i = getShipSpeed(shipCellData_i.getSpeed(), shipCategory_i);
	const double u_j = getShipSpeed(shipCellData_j.getSpeed(), shipCategory_j);

	if (u_i == 0 && u_j == 0) {
		LogVerbose("Cell interaction with shipCat_i=" << shipCategory_i.getUID() << ", and shipCat_j=" << shipCategory_j.getUID() << " both have zero speed.");
		return;
	}

	// At least one ship has uniform direction
	if (std::isnan(shipCellData_i.getDirection()) || std::isnan(shipCellData_j.getDirection())) {
		constexpr size_t numBins = GeneralModelData::numAngularBins();
		constexpr double numBinsInv = 1.0 / numBins;
		for (size_t i = 0; i < numBins; i++) {
			constexpr double dtheta = std::numbers::pi * numBinsInv;
			double theta = std::abs(sml::normalisedBalancedAngles(dtheta * i)); // theta is defined as a cross product and hence may not be larger than pi
			const double cosTheta = std::cos(theta);
			const double sinTheta = std::sin(theta);
			const double u_ij = std::sqrt(u_i * u_i + u_j * u_j - 2.0 * u_i * u_j * cosTheta);
			const auto collisionProjection = getCollisionProjection(cosTheta, sinTheta, u_i, u_j, u_ij, shipCategory_i, shipCategory_j);
			exposure += u_ij * collisionProjection.length;
			struckChance_i += collisionProjection.struckChance_i;
			struckChance_j += collisionProjection.struckChance_j;
		}
		// Multiply by chance
		exposure *= numBinsInv;	
		struckChance_i *= numBinsInv;
		struckChance_j *= numBinsInv;
	}
	else {
		// Two distinct directions
		double angle_i = shipCellData_i.getDirection();
		double angle_j = shipCellData_j.getDirection();
		double theta = std::abs(sml::normalisedBalancedAngles(angle_j - angle_i));
		const double cosTheta = std::cos(theta);
		const double sinTheta = std::sin(theta);
		const double u_ij = std::sqrt(u_i * u_i + u_j * u_j - 2.0 * u_i * u_j * cosTheta);
		const auto collisionProjection = getCollisionProjection(cosTheta, sinTheta, u_i, u_j, u_ij, shipCategory_i, shipCategory_j);
		exposure = u_ij * collisionProjection.length;
		struckChance_i = collisionProjection.struckChance_i;
		struckChance_j = collisionProjection.struckChance_j;
	}

	// Multiply by density and cell area
	exposure *= shipCellData_i.getDensity() * shipCellData_j.getDensity() * m_area;
}



void Cell::computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j)
{
	const auto& shipCategory_i = getShipCategory(meeting.i);
	const auto& shipCategory_j = getShipCategory(meeting.j);
	const auto& causationFactors = m_model.getCausationModel().getCausationFactors();

	const size_t index_i = causationFactors.getIndex(shipCategory_i.getUID());
	const size_t index_j = causationFactors.getIndex(shipCategory_j.getUID());

	events_i = meeting.struckChance_i * causationFactors.get(index_i)->getShipShipFactor() * meeting.exposures;
	events_j = meeting.struckChance_j * causationFactors.get(index_j)->getShipShipFactor() * meeting.exposures;
}



void Cell::computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j)
{
	// Grab meeting data
	const auto& pilotAssistanceZone = m_model.getMeasureModel().getPilotAssistanceZones();

	double pzone = 1.0;
	if (m_zoneScheme) {
		size_t zoneIndex = pilotAssistanceZone.getIndex(m_zoneScheme->getUID());
		if (zoneIndex == pilotAssistanceZone.size()) {
			LogError("pilotAssistance zone factor for zone " << m_zoneScheme->getUID() << " not found in the database.");
		}
		pzone = pilotAssistanceZone.get(zoneIndex)->getProbability();
	}

	collisions_i = meeting.events_i * pzone;
	collisions_j = meeting.events_j * pzone;
}



const tdb::ZoneScheme* Cell::findPilotAssistanceZone(const tdb::IndexedZones& zones, const tdb::IndexedVertices& vertices) const
{
	const tdb::ZoneScheme* zoneScheme = nullptr;
	for (const auto& zone : zones.getIndexedData()) {
		// Project coordinates and create a polygon
		sml::Polygon polygon;
		for (size_t i = 0; i < zone->getVertices().size() - 1; i++) {
			// The database has the last vertex as the first to close the loop, perhaps consider zones to be polygons from the start?
			const auto& vertexScheme = vertices.get(zone->getVertices()[i]);
			sml::Coordinate2D coord = { vertexScheme->getLatitude(), vertexScheme->getLongitude() };
			polygon.addVertex(sml::GeoUtils::projectToPlane(m_origin, coord));
		}

		// Check if all cell vertices are within the zone
		bool isInScheme = true;
		const tdb::ZoneScheme* candidateScheme = nullptr;
		for (size_t i = 0; i < m_polygon.getVertexCount(); i++) {
			if (!polygon.isInside(m_polygon[i])) {
				isInScheme = false;
				break;
			}
			else {
				if (zoneScheme == nullptr) {
					candidateScheme = zone.get();
				}
				else {
					LogWarning("Multiple zones defined around the link.");
				}
			}
		}

		// Found a fitting zone
		if (isInScheme) {
			zoneScheme = candidateScheme;
		}
	}
	return zoneScheme;
}

} // namespace sem

