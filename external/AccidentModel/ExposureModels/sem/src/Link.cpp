// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/Link.h>
#include <sem/ExposureModel.h>
#include <sml/Functions.h>
#include <sml/GeoUtils.h>
#include <sml/Polygon.h>

namespace sem
{
Link::Link(const size_t& index, const tdb::Data<tdb::LinkScheme>& linkdb, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDataBase)
	: base(index, linkdb, trafficDataBase.getTrafficModel())
	, m_model(model)
	, m_trafficDataBase(trafficDataBase)
	, m_zoneScheme(nullptr)
{
}



void Link::setup()
{
	const sml::Coordinate2D start = getCoordinates()[0];
	const sml::Coordinate2D end = getCoordinates()[1];

	// Project coordinates onto an xy-plane, taking the origin at half the link length
	m_origin = { start.lat + 0.5 * (end.lat - start.lat), start.lon + 0.5 * (end.lon - start.lon) };
	m_p0 = sml::GeoUtils::projectToPlane(m_origin, start);
	m_p1 = sml::GeoUtils::projectToPlane(m_origin, end);

	// Compute direction and length of the link
	sml::Vector2d direction = m_p1 - m_p0;
	double length = direction.norm();
	direction /= length;
	m_length = length;
	m_direction = direction;
	m_angle = std::atan2(direction[1], direction[0]);	// Used for the factor table? TODO get the right range from 0 to 2 pi

	// Compute left and right boundary
	double min = DBL_MAX;
	double max = -DBL_MAX;
	const auto& shipLinkData = m_model.getDataModel().getIndexedShipLinkData();
	for (auto index : m_dataIndices) {
		const auto& data = shipLinkData.get(index);
		double mu = data->getMu();
		double sigma = data->getSigma();
		double leftBoundary = mu + 3.0 * sigma;
		double rightBoundary = mu - 3.0 * sigma;
		if (rightBoundary < min) {
			min = rightBoundary;
		}
		if (leftBoundary > max) {
			max = leftBoundary;
		}
	}
	m_leftBoundary = max;
	m_rightBoundary = min;

	m_zoneScheme = findPilotAssistanceZone(m_trafficDataBase.getAreaModel().getZones(), m_trafficDataBase.getAreaModel().getVertices());

	createRectangle();
	
	createPolygon();
}



void Link::computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j)
{
	// Grab the meeting data
	const auto& shipLinkData_i = getShipLinkData(meeting.i);
	const auto& shipLinkData_j = getShipLinkData(meeting.j);
	const auto& shipCategory_i = getShipCategory(meeting.i);
	const auto& shipCategory_j = getShipCategory(meeting.j);

	// Compute exposure
	exposure = 0.0;
	struckChance_i = 0.0;
	struckChance_j = 0.0;
	const double u_i = getShipSpeed(shipLinkData_i.getSpeed(), shipCategory_i);
	const double u_j = getShipSpeed(shipLinkData_j.getSpeed(), shipCategory_j);
	const double u_ij = u_j - u_i;
	if (u_ij < 0.0) {
		return;
	}
	double mu_ij = shipLinkData_i.getMu() - shipLinkData_j.getMu();
	double sigma_i = shipLinkData_i.getSigma();
	double sigma_j = shipLinkData_j.getSigma();
	double sigma_ij = std::sqrt(sigma_i * sigma_i + sigma_j * sigma_j);
	double b_ij = 0.5 * (shipCategory_i.getWidth() + shipCategory_j.getWidth());
	const double q_i = shipLinkData_i.getShipRate();
	const double q_j = shipLinkData_j.getShipRate();
	double p_ij = sml::cdf_normalDistribution(b_ij, mu_ij, sigma_ij) - sml::cdf_normalDistribution(-b_ij, mu_ij, sigma_ij);
	exposure = m_length * p_ij * q_i * q_j / (u_i * u_j) * u_ij;
	struckChance_i = 1.0;
	struckChance_j = 0.0; // Perfectly safe this ship! Its better to be the quickest on the link :D

	// Indicator
	LogVerbose("Overtaking model | "
		<< "link: " << getUID()
		<< ", shipA data: " << shipLinkData_i.getUID()
		<< ", shipB data: " << shipLinkData_j.getUID()
		<< ", length: " << m_length
		<< ", p_ij: " << p_ij
		<< ", q_i: " << q_i
		<< ", q_j: " << q_j
		<< ", u_i: " << u_i
		<< ", u_j: " << u_j
		<< ", u_ij: " << u_ij
		<< ", b_ij: " << b_ij
		<< ", result: " << m_length * p_ij * q_i * q_j / (u_i * u_j) * u_ij);
	return;
}



void Link::computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j)
{
	const auto& shipCategory_i = *m_trafficModel.getShipCategories().get(m_shipCategoryDataIndices[meeting.i]);
	const auto& causationFactors = m_model.getCausationModel().getCausationFactors();
	const size_t index = causationFactors.getIndex(shipCategory_i.getUID());
	double factor = 0.0;
	if (index == causationFactors.size()) {
		LogError("Error in database discovered, can't find ship category " << shipCategory_i.getUID() << " in the ShipShipFactors database.");
	}
	else {
		factor = causationFactors.get(index)->getShipShipFactor();
	}
	LogVerbose("Link: " << getUID() << ", angle: " << m_angle << ", ship_i: " << shipCategory_i.getUID() << " causation factor : " << factor);
	events_i = meeting.struckChance_i * factor * meeting.exposures;
	events_j = 0.0; 
}



void Link::computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j)
{
	// Grab meeting data
	//const auto& pilotAssistanceShip = m_model.getMeasureModel().getPilotAssistanceShip();
	const auto& pilotAssistanceZones = m_model.getMeasureModel().getPilotAssistanceZones();
	//const auto& shipCategory_i = *getShipCategory(meeting.i);
	//const auto& shipCategory_j = *getShipCategory(meeting.j);
	//
	//size_t shipFactorIndex_i = pilotAssistanceShip.getIndex(shipCategory_i.getUID());
	//size_t shipFactorIndex_j = pilotAssistanceShip.getIndex(shipCategory_i.getUID());
	//if (shipFactorIndex_i == pilotAssistanceShip.size()) {
	//	LogError << "pilotAssistance ship factor for class " << shipCategory_i.getUID() << " not found in the database.";
	//}
	//if (shipFactorIndex_j == pilotAssistanceShip.size()) {
	//	LogError << "pilotAssistance ship factor for class " << shipCategory_j.getUID() << " not found in the database.";
	//}
	//
	//double pi = pilotAssistanceShip.get(shipFactorIndex_i)->getProbability();
	//double pj = pilotAssistanceShip.get(shipFactorIndex_j)->getProbability();
	// Note: measure based on ship class is currently not yet finalised
	double pzone = 1.0;
	if (m_zoneScheme) {
		size_t zoneIndex = pilotAssistanceZones.getIndex(m_zoneScheme->getUID());
		if (zoneIndex == pilotAssistanceZones.size()) {
			LogError("pilotAssistance zone factor for zone " << m_zoneScheme->getUID() << " not found in the database.");
		}
		pzone = pilotAssistanceZones.get(zoneIndex)->getProbability();
	}
	
	collisions_i = meeting.events_i * pzone;
	collisions_j = 0.0;
}



void Link::createRectangle()
{
	std::vector<sml::Vector2d> points;
	points.push_back({ 0.0, m_leftBoundary });
	points.push_back({ 0.0, m_rightBoundary });
	points.push_back({ m_length, m_rightBoundary });
	points.push_back({ m_length, m_leftBoundary });
	points.push_back({ 0.0, m_leftBoundary });

	// Transform to local plane
	double cos = std::cos(m_angle);
	double sin = std::sin(m_angle);
	for (size_t i = 0; i < points.size(); i++) {
		double px = points[i][0];
		double py = points[i][1];
		points[i][0] = px * cos - py * sin + m_p0[0];
		points[i][1] = px * sin + py * cos + m_p0[1];
	}

	// Project back to WGS84 coordinates
	for (auto point : points) {
		m_rectangle.push_back(sml::GeoUtils::projectionToCoordinate(m_origin, point));
	}
}



const tdb::ZoneScheme* Link::findPilotAssistanceZone(const tdb::IndexedZones& zones, const tdb::IndexedVertices& vertices) const
{
	const tdb::ZoneScheme* zoneScheme = nullptr;
	for (const auto& zone : zones.getIndexedData()) {
		//TODO coarse grained check could speed this up
		
		// Project coordinates and create a polygon
		sml::Polygon polygon;
		for (size_t i = 0; i < zone->getVertices().size() - 1; i++) { 
			// The database has the last vertex as the first to close the loop, perhaps consider zones to be polygons from the start?
			const auto& vertexScheme = vertices.get(zone->getVertices()[i]);
			sml::Coordinate2D coord = { vertexScheme->getLatitude(), vertexScheme->getLongitude()};
			polygon.addVertex(sml::GeoUtils::projectToPlane(m_origin, coord));
		}

		// Check if the link is within the zone
		if (polygon.isInside(m_p0) && polygon.isInside(m_p1)) {
			if (zoneScheme == nullptr) {
				zoneScheme = zone.get();
			} else {
				LogWarning("Multiple zones defined around the link.");
			}
		}
	}
	return zoneScheme;
}



// Function follows the outer tangent of https://en.wikipedia.org/wiki/Tangent_lines_to_circles
void Link::createPolygon()
{
	// Get the waypoint radii
	const double r0 = getWaypointStart()->getRadius();
	const double r1 = getWaypointEnd()->getRadius();

	// Generate local intersection points of each waypoint
	double beta = std::asin((r1 - r0) / m_length);
	sml::Vector2d p2 = { -r0 * std::sin(beta), r0 * std::cos(beta) };
	sml::Vector2d p3 = { -r1 * std::sin(beta), r1 * std::cos(beta) };

	// Generate local angle ranges of each waypoint
	constexpr double gammaStart0 = std::numbers::pi;
	const double gammaEnd0 = std::atan2(p2[1], p2[0]);
	const double gammaStart1 = std::atan2(p3[1], p3[0]);
	constexpr double gammaEnd1 = 0.0;

	// Generate points in local frame of the start waypoint
	constexpr size_t numSegments = 8;
	const double dgamma0 = (gammaEnd0 - gammaStart0) / (numSegments - 1);
	const double dgamma1 = (gammaEnd1 - gammaStart1) / (numSegments - 1);
	std::vector<sml::Vector2d> points;
	for (size_t i = 0; i < numSegments; i++) { 	// Top left
		double gamma = gammaStart0 + i * dgamma0;
		points.push_back({ r0 * std::cos(gamma), r0 * std::sin(gamma) });
	}
	for (size_t i = 0; i < numSegments; i++) { 	// Top right
		double gamma = gammaStart1 + i * dgamma1;
		points.push_back({ r1 * std::cos(gamma) + m_length, r1 * std::sin(gamma) });
	}
	for (size_t i = 0; i < numSegments; i++) {
		size_t index = 2 * numSegments - (i + 1);
		points.push_back({ points[index][0], -points[index][1] });
	}
	for (size_t i = 0; i < numSegments; i++) {
		size_t index = numSegments - (i + 1);
		points.push_back({ points[index][0], -points[index][1] });
	}

	// Transform from local start waypoint frame to global projected frame
	// TODO improve this when/if a matrix is introduced
	double cos = std::cos(m_angle);
	double sin = std::sin(m_angle);
	for (size_t i = 0; i < points.size(); i++) {
		double px = points[i][0];
		double py = points[i][1];
		points[i][0] = px * cos - py * sin + m_p0[0];
		points[i][1] = px * sin + py * cos+ m_p0[1];
	}

	// Project back to WGS84 coordinates
	for (auto point : points) {
		m_polygon.push_back(sml::GeoUtils::projectionToCoordinate(m_origin, point));
	}
}


} // namespace sem 

