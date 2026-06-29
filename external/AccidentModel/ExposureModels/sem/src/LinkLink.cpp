// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/LinkLink.h>
#include <sem/Link.h>
#include <sem/ExposureModel.h>
#include <sml/Functions.h>
#include <sml/Polygon.h>

namespace sem
{

LinkLink::LinkLink(const sim::Link& linkA, const sim::Link& linkB, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase)
	: base(linkA, linkB, trafficDatabase.getTrafficModel().getShipCategories())
	, m_linkA(static_cast<const sem::Link&>(linkA))
	, m_linkB(static_cast<const sem::Link&>(linkB))
	, m_model(model)
	, m_trafficDatabase(trafficDatabase)
	, m_headOn(false)
	, m_zoneScheme(nullptr)
{
}



void LinkLink::setup()
{
	const auto& wpAStart = m_linkA.getLinkScheme()->getStart();
	const auto& wpAEnd = m_linkA.getLinkScheme()->getEnd();
	const auto& wpBStart = m_linkB.getLinkScheme()->getStart();
	const auto& wpBEnd = m_linkB.getLinkScheme()->getEnd();
	if (wpAEnd == wpBStart && wpBEnd == wpAStart) {
		m_headOn = true;
	}

	findPilotAssistanceZone(m_trafficDatabase.getAreaModel().getZones(), m_trafficDatabase.getAreaModel().getVertices());

	m_pcollision.resize(m_linkA.getNumShips(), std::vector<double>(m_linkB.getNumShips(), 0.0));
}



void LinkLink::computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j)
{
	exposure = 0.0;
	struckChance_i = 0.0;
	struckChance_j = 0.0;
	if (m_headOn) {
		return headOn(meeting, exposure, struckChance_i, struckChance_j);
	}
	else {
		// Based on the angle, grab the correct exposure model
		constexpr double crossingLimLow = 0.17453292519;
		constexpr double crossingLimHigh = M_PI - crossingLimLow;
		double angle = getAngle();
		if (angle == 0.0) {
			return overtaking(meeting, exposure, struckChance_i, struckChance_j);
		}
		else if (angle > crossingLimLow && angle < crossingLimHigh) {
			return crossing(meeting, exposure, struckChance_i, struckChance_j);
		}
		else {
			LogWarning("No link-link crossing model found for angle " << angle << " between " << m_linkA.getUID() << " and " << m_linkB.getUID());
		}
	}
}



void LinkLink::computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j)
{
	const auto& shipCategory_i = m_linkA.getShipCategory(meeting.i);
	const auto& shipCategory_j = m_linkB.getShipCategory(meeting.j);
	const auto& causationFactors = m_model.getCausationModel().getCausationFactors();

	const size_t index_i = causationFactors.getIndex(shipCategory_i.getUID());
	const size_t index_j = causationFactors.getIndex(shipCategory_j.getUID());
	if (index_i == causationFactors.size()) {
		LogError("Error in database discovered, can't find ship category " << shipCategory_i.getUID() << " in the ShipShipFactors database.");
	}
	if (index_j == causationFactors.size()) {
		LogError("Error in database discovered, can't find ship category " << shipCategory_j.getUID() << " in the ShipShipFactors database.");
	}

	events_i = meeting.struckChance_i * causationFactors.get(index_i)->getShipShipFactor() * meeting.exposures;
	events_j = meeting.struckChance_j * causationFactors.get(index_j)->getShipShipFactor() * meeting.exposures;
}



void LinkLink::computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j)
{
	// Grab meeting data
	//const auto& pilotAssistanceShip = m_model.getMeasureModel().getPilotAssistanceShip();
	const auto& pilotAssistanceZones = m_model.getMeasureModel().getPilotAssistanceZones();
	//const auto& shipCategory_i = *m_linkA.getShipCategory(meeting.i);
	//const auto& shipCategory_j = *m_linkB.getShipCategory(meeting.j);
	//
	//size_t shipFactorIndex_i = pilotAssistanceShip.getIndex(shipCategory_i.getUID());
	//size_t shipFactorIndex_j = pilotAssistanceShip.getIndex(shipCategory_i.getUID());
	//if (shipFactorIndex_i == pilotAssistanceShip.size()) {
	//	LogError("pilotAssistance ship factor for class " << shipCategory_i.getUID() << " not found in the database.");
	//}
	//if (shipFactorIndex_j == pilotAssistanceShip.size()) {
	//	LogError("pilotAssistance ship factor for class " << shipCategory_j.getUID() << " not found in the database.");
	//}
	//
	//double pi = pilotAssistanceShip.get(shipFactorIndex_i)->getProbability();
	//double pj = pilotAssistanceShip.get(shipFactorIndex_j)->getProbability();
	// Class dependent pilot assistance is for now disabled

	double pzone = 1.0;
	if (m_zoneScheme) {
		size_t zoneIndex = pilotAssistanceZones.getIndex(m_zoneScheme->getUID());
		if (zoneIndex == pilotAssistanceZones.size()) {
			LogError("pilotAssistance zone factor for zone " << m_zoneScheme->getUID() << " not found in the database.");
		}
		pzone = pilotAssistanceZones.get(zoneIndex)->getProbability();
	}

	collisions_i = meeting.events_i * pzone;
	collisions_j = meeting.events_j * pzone;
}



void LinkLink::crossing(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j)
{
	// Grab the meeting data
	const auto& shipLinkData_i = m_linkA.getShipLinkData(meeting.i);
	const auto& shipLinkData_j = m_linkB.getShipLinkData(meeting.j);
	const auto& shipCategory_i = m_linkA.getShipCategory(meeting.i);
	const auto& shipCategory_j = m_linkB.getShipCategory(meeting.j);

	// Grab the values
	const double u_i = getShipSpeed(shipLinkData_i.getSpeed(), shipCategory_i);
	const double u_j = getShipSpeed(shipLinkData_j.getSpeed(), shipCategory_j);
	const double q_i = shipLinkData_i.getShipRate();
	const double q_j = shipLinkData_j.getShipRate();
	const double angle = getAngle();
	const double cosAngle = std::cos(angle);
	const double sinAngle = std::sin(angle);
	const double u_ij = std::sqrt(u_i * u_i + u_j * u_j - 2.0 * u_i * u_j * cosAngle);
	const auto collisionProjection = getCollisionProjection(cosAngle, sinAngle, u_i, u_j, u_ij, shipCategory_i, shipCategory_j);

	const double p_ij = collisionProjection.length / sinAngle;

	// return the result
	exposure = p_ij * q_i * q_j / (u_i * u_j) * u_ij;
	struckChance_i = collisionProjection.struckChance_i;
	struckChance_j = collisionProjection.struckChance_j;

	LogVerbose("Crossing model | "
		<< "linkA: " << m_linkA.getUID()
		<< ", linkB: " << m_linkB.getUID()
		<< ", shipA data: " << shipLinkData_i.getUID()
		<< ", shipB data: " << shipLinkData_j.getUID()
		<< ", p_ij: " << p_ij
		<< ", q_i: " << q_i
		<< ", q_j: " << q_j
		<< ", u_i: " << u_i
		<< ", u_j: " << u_j
		<< ", u_ij: " << u_ij
		<< ", result: " << exposure);
}



void LinkLink::headOn(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j)
{
	// Grab the meeting data
	const auto& shipLinkData_i = m_linkA.getShipLinkData(meeting.i);
	const auto& shipLinkData_j = m_linkB.getShipLinkData(meeting.j);
	const auto& shipCategory_i = m_linkA.getShipCategory(meeting.i);
	const auto& shipCategory_j = m_linkB.getShipCategory(meeting.j);

	const double u_i = getShipSpeed(shipLinkData_i.getSpeed(), shipCategory_i);
	const double u_j = getShipSpeed(shipLinkData_j.getSpeed(), shipCategory_j);
	
	const double length = getLinkA().getLength();
	const double q_i = shipLinkData_i.getShipRate();
	const double q_j = shipLinkData_j.getShipRate();
	const double u_ij = u_i + u_j;
	const double mu_ij = shipLinkData_i.getMu() + shipLinkData_j.getMu();
	const double sigma_i = shipLinkData_i.getSigma();
	const double sigma_j = shipLinkData_j.getSigma();
	const double sigma_ij = std::sqrt(sigma_i * sigma_i + sigma_j * sigma_j);
	const double b_ij = 0.5 * (shipCategory_i.getWidth() + shipCategory_j.getWidth());
	const double p_ij = sml::cdf_normalDistribution(b_ij, mu_ij, sigma_ij) - sml::cdf_normalDistribution(-b_ij, mu_ij, sigma_ij);
	exposure = length * p_ij * q_i * q_j / (u_i * u_j) * u_ij;
	struckChance_i = 0.5;
	struckChance_j = 0.5;
	
	LogVerbose("Headon model: "
		<< "linkA: " << m_linkA.getUID()
		<< ", linkB: " << m_linkB.getUID()
		<< ", shipA data: " << shipLinkData_i.getUID()
		<< ", shipB data: " << shipLinkData_j.getUID()
		<< ", length: " << length 
		<< ", p_ij: " << p_ij
		<< ", q_i: " << q_i
		<< ", q_j: " << q_j
		<< ", u_i: " << u_i
		<< ", u_j: " << u_j
		<< ", result: " << exposure);
}



void LinkLink::overtaking(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j)
{
	LogError("Two identical links are not expected to exist");
	exposure = 0.0;
	struckChance_i = 0.0;
	struckChance_j = 0.0;
}



void LinkLink::findPilotAssistanceZone(const tdb::IndexedZones& zones, const tdb::IndexedVertices& vertices) 
{
	if (m_headOn) {
		// The whole link should be within a zone
		m_zoneScheme = m_linkA.getZoneScheme();
	}
	else {
		// Perform in lat/lon space, since the crossing point is at the end way point of both links
		sml::Vector2d point = { m_linkA.getCoordinates()[1].lat, m_linkA.getCoordinates()[1].lon };
		std::vector<tdb::ZoneScheme> zoneSchemes;
		for (const auto& zone : zones.getIndexedData()) {
			//TODO coarse grained check could speed this up

			// Project coordinates and create a polygon
			sml::Polygon polygon;
			for (size_t i = 0; i < zone->getVertices().size() - 1; i++) {
				// The database has the last vertex as the first to close the loop, perhaps consider zones to be polygons from the start?
				const auto& vertexScheme = vertices.get(zone->getVertices()[i]);
				polygon.addVertex({vertexScheme->getLatitude(), vertexScheme->getLongitude() });
			}

			// Check if the link is within the zone
			if (polygon.isInside(point)) {
				if (m_zoneScheme == nullptr) {
					m_zoneScheme = zone.get();
				}
				else {
					LogWarning("Multiple zones defined around the link.");
				}
			}
		}
	}
}

} // namespace sem

