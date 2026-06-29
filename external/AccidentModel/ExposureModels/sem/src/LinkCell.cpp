// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/LinkCell.h>
#include <sem/ExposureModel.h>
#include <sml/GeoUtils.h>
#include <sml/Functions.h>
#include <cmath>

namespace sem
{
LinkCell::LinkCell(const sim::Link& link, const sim::Cell& cell, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase)
	: base(link, cell)
	, m_link(static_cast<const sem::Link&>(link))
	, m_cell(static_cast<const sem::Cell&>(cell))
	, m_model(model)
	, m_trafficDatabase(trafficDatabase)
	, m_totalArea(0.0)
{
}



void LinkCell::setup()
{
	// Transform the intersecions into strips to compute
	sml::Vector2d l0 = sml::GeoUtils::projectToPlane(m_cell.getOrigin(), m_link.getCoordinates()[0]);
	sml::Vector2d l1 = sml::GeoUtils::projectToPlane(m_cell.getOrigin(), m_link.getCoordinates()[1]);
	sml::Vector2d dir = l1 - l0;
	dir.normalise();
	sml::Vector2d dirOrthogonal(-dir[1], dir[0]);

	double width = std::abs(m_link.getLeftBoundary() - m_link.getRightBoundary());
	m_dwidth = width / GeneralModelData::numSubCellLink();

	m_angle = std::atan2(dir[1], dir[0]);

	for (size_t i = 0; i < GeneralModelData::numSubCellLink(); i++) {
		// Create sublink
		double offset0 = -0.5 * width + m_dwidth * i;
		double offset1 = offset0 + m_dwidth;
		sml::Polygon pSubLink;
		pSubLink.addVertex(l0 - dirOrthogonal * offset0);
		pSubLink.addVertex(l0 - dirOrthogonal * offset1);
		pSubLink.addVertex(l1 - dirOrthogonal * offset1);
		pSubLink.addVertex(l1 - dirOrthogonal * offset0);

		// Get intersection
		std::vector<sml::Polygon> subCellLinks = m_cell.getPolygon().getIntersectionWith(pSubLink);
		for (const auto& subCellLink : subCellLinks) {
			double area = subCellLink.getArea();
			if (area > 0.0) {
				// Create subCell
				m_subCells.push_back({ area, offset0, offset1});
				m_totalArea += area;
				//std::cout << "subcell| area: " << area << ", length: " << area / dw << ", offset" << 0.5 * (offset0 + offset1) << std::endl;
			}
		}
	}
}



void LinkCell::computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j)
{
	exposure = 0.0;
	struckChance_i = 0.0;
	struckChance_j = 0.0;

	// Grab the meeting data
	const auto& shipCategory_i = m_link.getShipCategory(meeting.i);
	const auto& shipCategory_j = m_cell.getShipCategory(meeting.j);
	const auto& shipLinkData_i = m_link.getShipLinkData(meeting.i);
	const auto& shipCellData_j = m_cell.getShipCellData(meeting.j);

	const double u_i = getShipSpeed(shipLinkData_i.getSpeed(), shipCategory_i);
	const double u_j = getShipSpeed(shipCellData_j.getSpeed(), shipCategory_j);
	const double mu = shipLinkData_i.getMu();
	const double sigma = shipLinkData_i.getSigma();

	for (const auto& subCell : m_subCells) {
		double subExposure = 0.0;
		double subStruckChance_i = 0.0;
		double subStruckChance_j = 0.0;

		double probability = sml::cdf_normalDistribution(subCell.offset1, mu, sigma) - sml::cdf_normalDistribution(subCell.offset0, mu, sigma);
		double density_i = shipLinkData_i.getShipRate() * probability / (u_i * m_dwidth);

		if (std::isnan(shipCellData_j.getDirection())) {
			constexpr size_t numBins = GeneralModelData::numAngularBins();
			constexpr double numBinsInv = 1.0 / numBins;
			for (size_t i = 0; i < numBins; i++) {
				constexpr double dtheta = std::numbers::pi * numBinsInv;
				double theta = std::abs(sml::normalisedBalancedAngles(dtheta * i)); // theta is defined as a cross product and hence may not be larger than pi
				const double cosTheta = std::cos(theta);
				const double sinTheta = std::sin(theta);
				const double u_ij = std::sqrt(u_i * u_i + u_j * u_j - 2.0 * u_i * u_j * cosTheta);
				const auto collisionProjection = getCollisionProjection(cosTheta, sinTheta, u_i, u_j, u_ij, shipCategory_i, shipCategory_j);
				subExposure += u_ij * collisionProjection.length;
				subStruckChance_i += collisionProjection.struckChance_i;
				subStruckChance_j += collisionProjection.struckChance_j;
			}

			// Multiply by chance
			subExposure *= numBinsInv;
			subStruckChance_i *= numBinsInv;
			subStruckChance_j *= numBinsInv;
		}
		else {
			// Two distinct directions
			double angle_j = shipCellData_j.getDirection();
			double theta = std::abs(sml::normalisedBalancedAngles(angle_j - m_angle));
			const double cosTheta = std::cos(theta);
			const double sinTheta = std::sin(theta);
			const double u_ij = std::sqrt(u_i * u_i + u_j * u_j - 2.0 * u_i * u_j * cosTheta);
			const auto collisionProjection = getCollisionProjection(cosTheta, sinTheta, u_i, u_j, u_ij, shipCategory_i, shipCategory_j);
			subExposure = u_ij * collisionProjection.length;
			subStruckChance_i = collisionProjection.struckChance_i;
			subStruckChance_j = collisionProjection.struckChance_j;
		}

		LogVerbose("subExposure=" << subExposure
			<< "density_i = " << density_i
			<< ", density_j=" << shipCellData_j.getDensity()
			<< ", area=" << subCell.area);

		subExposure *= density_i * shipCellData_j.getDensity() * subCell.area;

		// Dont forget the probability of the sublink, is this fixed when computing the density?
		double ratio = subCell.area / m_totalArea;
		exposure += subExposure;
		struckChance_i += ratio * subStruckChance_i;
		struckChance_j += ratio * subStruckChance_j;
	}
	LogVerbose("Total exposure=" << exposure);
}



void LinkCell::computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j)
{
	const auto& shipCategory_i = m_link.getShipCategory(meeting.i);
	const auto& shipCategory_j = m_cell.getShipCategory(meeting.j);
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



void LinkCell::computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j)
{
	const auto& pilotAssistanceZones = m_model.getMeasureModel().getPilotAssistanceZones();
	double pzone = 1.0;
	if (m_link.getZoneScheme() && m_cell.getZoneScheme()) {
		if (m_link.getZoneScheme() == m_cell.getZoneScheme()) {
			size_t zoneIndex = pilotAssistanceZones.getIndex(m_link.getZoneScheme()->getUID());
			if (zoneIndex == pilotAssistanceZones.size()) {
				LogError("pilotAssistance zone factor for zone " << m_link.getZoneScheme()->getUID() << " not found in the database.");
			}
			pzone = pilotAssistanceZones.get(zoneIndex)->getProbability();
		}
	}

	collisions_i = meeting.events_i * pzone;
	collisions_j = meeting.events_j * pzone;
}

} // namespace sem

