// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/LinkObject.h>
#include <sem/DataModel.h>
#include <sem/ExposureModel.h>
#include <sml/Vector.h>
#include <sml/Functions.h>
#include <sml/GeoUtils.h>
#include <numbers>
#include <tdb/AreaSchemes.h>
#include <ranges>
#include <algorithm>

namespace sem
{
LinkObject::LinkObject(const sim::Link& link, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase, const tdb::WeatherstationScheme& wss)
	: base(link) 
	, m_model(model)
	, m_trafficDataBase(trafficDatabase)
	, m_driftModels(model.getDriftModels(wss))
	, m_driftDiscretisation(model.getDriftDiscretisation(wss))
	, m_link(dynamic_cast<const sem::Link&>(link))
	, m_linkAngle(0.0)
{
}



void LinkObject::setup()
{
	// Model parameters - should be moved somewhere else (dataModel I guess)
	bool useShielding = false;		// For now only true or false for all objects are supported
	constexpr double maxSubLinkBinSize = GeneralModelData::maxSubLinkBinSize();

	// Group objects based on shielding property
	// TODO this is not yet fully implemented
	// TODO do I want this already separated in the InteractionModel?
	if (useShielding) {
		for (auto object : getObjects()) {
			m_shieldingObjects.push_back(object);
		}
	} else {
		for (auto object : getObjects()) {
			m_nonShieldingObjects.push_back(object);
		}
	}

	// Project all relevant object coordinates to vertices for the given origin of this LinkObject
	projectCoordinates(m_shieldingObjects, m_verticesShieldingObjects);
	projectCoordinates(m_nonShieldingObjects, m_verticesNonShieldingObjects);

	// Discretise traffic area by subdividing them in sub links
	double width = std::abs(getLink().getLeftBoundary() - getLink().getRightBoundary());
	size_t numSubLinks = std::ceil(width / maxSubLinkBinSize);
	m_subLinkWidth = width / numSubLinks;
	sml::Vector2d dirLink = getP1() - getP0();
	m_linkAngle = sml::normalisedPositiveAngles(std::atan2(dirLink[1], dirLink[0]));
	dirLink /= dirLink.norm();
	sml::Vector2d normal(-dirLink[1], dirLink[0]);
	for (size_t i = 0; i < numSubLinks; i++) {
		double offset = getLink().getRightBoundary() + (i + 0.5) * m_subLinkWidth;
		sml::Vector2d diff = normal * offset;
		sml::Vector2d p0 = getP0() + diff;
		sml::Vector2d p1 = getP1() + diff;
		m_subLinks.push_back(SubLinkObject(p0, p1, offset, *this));
		LogVerbose("LinkObject of link" << getLink().getUID() << " created sublink_" << i << " | p0: " << p0 << ", p1: " << p1 << ", diff: " << diff << ", offset: " << offset);
	}

	// Create ram discretisation
	size_t numBins = 64;
	double linkAngle = std::atan2(dirLink[1], dirLink[0]);
	double minAngle = linkAngle - 3 * GeneralModelData::ramSigma();
	double stepSize = 6 * GeneralModelData::ramSigma() / numBins;
	for (size_t i = 0; i < numBins + 1; i++) {
		m_ramDiscretisation.push_back(sml::normalisedPositiveAngles(minAngle + stepSize * i));
	}
	std::sort(m_ramDiscretisation.begin(), m_ramDiscretisation.end());

	// Generate the meetings
	m_meetingsDrift.resize(getLink().getNumShips());
	for (auto& meeting : m_meetingsDrift) {
		meeting.exposures.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.collisions.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.globalObjectIndex.resize(m_nonShieldingObjects.size());
	}
	m_meetingsRam.resize(getLink().getNumShips());
	for (auto& meeting : m_meetingsRam) {
		meeting.exposures.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.collisions.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.globalObjectIndex.resize(m_nonShieldingObjects.size());
	}

	base::setup();
	setNumShips(getLink().getNumShips());
	setNumObjects(m_nonShieldingObjects.size());
}



void LinkObject::prepareExposureComputation()
{
	// Set up the subLinks
	for (auto& subLink : m_subLinks) {
		subLink.setup();
		LogVerbose("LinkObject of link " << getLink().getUID() << " sub link adds " << subLink.getNumProjectionsDrift() << " drift projections and " << subLink.getNumProjectionsRam() << " ram projections.");
	}

	// Compute the detailed exposure data for each ship
	//LogInfo("Computing LinkObject detailed exposures for link " << getLink().getUID());
	for (size_t i = 0; i < m_link.getNumShips(); i++) {
		const auto& shipCategory_i = m_link.getShipCategory(i);
		const auto& shipLinkData_i = m_link.getShipLinkData(i);
		const double mu = shipLinkData_i.getMu();
		const double sigma = shipLinkData_i.getSigma();

		const double draft = shipCategory_i.getDraft();
		const double height = shipCategory_i.getHeight();

		const auto& shipDriftModel_i = m_driftModels[m_link.getShipCategoryIndex(i)];
		const double elevationMin = -draft;
		const double elevationMax = -draft + height;


		// Perform the detailed computation!
		for (auto& subLink : m_subLinks) {
			// Compute link probability
			double leftBoundary = subLink.getOffset() - 0.5 * m_subLinkWidth;
			double rightBoundary = leftBoundary + m_subLinkWidth;
			double pSubLink = sml::cdf_normalDistribution(rightBoundary, mu, sigma) - sml::cdf_normalDistribution(leftBoundary, mu, sigma);

			subLink.computeExposuresDrift(pSubLink, elevationMin, elevationMax, shipDriftModel_i, m_meetingsDrift[i]);
			subLink.computeExposuresRam(pSubLink, elevationMin, elevationMax, m_meetingsRam[i]);
		}
	}

	// Clean up the subLinks
	for (auto& subLink : m_subLinks) {
		subLink.cleanup();
	}
}



void LinkObject::prepareEventComputation()
{
	// Nothing to be done here
}




void LinkObject::prepareCollisionComputation()
{
	// Set up the subLinks
	for (auto& subLink : m_subLinks) {
		subLink.setup();
	}

	// Compute the detailed exposure data for each ship
	LogInfo("Computing LinkObject detailed measures for link " << getLink().getUID());
	for (auto& subLink : m_subLinks) {
		for (size_t i = 0; i < m_link.getNumShips(); i++) {
			const auto& shipCategory = m_link.getShipCategory(i);
			const auto& shipDriftModel = m_driftModels[m_link.getShipCategoryIndex(i)];

			const double length = shipCategory.getLength();
			const double draft = shipCategory.getDraft();
			const double height = shipCategory.getHeight();

			const double elevationMin = -draft;
			const double elevationMax = -draft + height;

			// Compute link probability
			const auto& shipLinkData_i = m_link.getShipLinkData(i);
			const double mu = shipLinkData_i.getMu();
			const double sigma = shipLinkData_i.getSigma();
			double leftBoundary = subLink.getOffset() - 0.5 * m_subLinkWidth;
			double rightBoundary = leftBoundary + m_subLinkWidth;
			double pSubLink = sml::cdf_normalDistribution(rightBoundary, mu, sigma) - sml::cdf_normalDistribution(leftBoundary, mu, sigma);

			// Perform the detailed computation!
			subLink.computeCollisionsDrift(pSubLink, elevationMin, elevationMax, shipDriftModel, m_meetingsDrift[i]);
			subLink.computeCollisionsRam(pSubLink, elevationMin, elevationMax, length, m_meetingsRam[i]);
		}
	}

	// Clean up the subLinks
	for (auto& subLink : m_subLinks) {
		subLink.cleanup();
	}
}



void LinkObject::computeExposure(const sim::ShipObjectMeeting& meeting, double& driftExposure, double& ramExposure)
{
	// Aggregate detailed results by summing up all contributions for a given ship and object index 
	//LogInfo("Aggregating LinkObject exposures results for link " << getLink().getUID());
	driftExposure = 0.0;
	ramExposure = 0.0;

	// Ship specific data
	const auto& shipLinkData_i = m_link.getShipLinkData(meeting.i);
	const double q = shipLinkData_i.getShipRate();
	const double mu = shipLinkData_i.getMu();
	const double sigma = shipLinkData_i.getSigma();

	// Compute the drift exposure
	const auto& meetingDrift_i = m_meetingsDrift[meeting.i];
	for (size_t localObjectIndex = 0; localObjectIndex < meetingDrift_i.globalObjectIndex.size(); localObjectIndex++) {
		if (meetingDrift_i.globalObjectIndex[localObjectIndex] == meeting.j) {
			driftExposure += q * meetingDrift_i.exposures[localObjectIndex];
		}
	}

	// Compute the ram exposure
	const auto& meetingRam_i = m_meetingsRam[meeting.i];
	for (size_t localObjectIndex = 0; localObjectIndex < meetingRam_i.globalObjectIndex.size(); localObjectIndex++) {
		if (meetingRam_i.globalObjectIndex[localObjectIndex] == meeting.j) {
			ramExposure += q * meetingRam_i.exposures[localObjectIndex];
		}
	}
}



void LinkObject::computeEvents(const sim::ShipObjectMeeting& meeting, double& driftEvents, double& ramEvents)
{
	// Grab databases
	const auto& shipCategory_i = m_link.getShipCategory(meeting.i);
	const auto& causationFactors = m_model.getCausationModel().getCausationFactors();

	// Get the causation factor
	const size_t index_i = causationFactors.getIndex(shipCategory_i.getUID());
	if (index_i == causationFactors.size()) {
		LogError("Ship class " << shipCategory_i.getUID() << " does not have an entry in the causation factor database.");
	}
	const auto& entry = causationFactors.get(index_i);
	const double driftFactor = entry->getDriftFactor();
	const double ramFactor = entry->getRamFactor();

	// Apply to sublink data
	m_meetingsDrift[meeting.i].causationFactor = driftFactor;
	m_meetingsRam[meeting.i].causationFactor = ramFactor;

	// Apply to aggregated data
	// Note: since this factor is not -inside- the for-loops, the aggregated results are identical to the results when aggregating the details events
	driftEvents = driftFactor * meeting.driftExposure;
	ramEvents = ramFactor * meeting.ramExposure;
}



void LinkObject::computeCollisions(const sim::ShipObjectMeeting& meeting, double& driftCollisions, double& ramCollisions)
{
	// Ship specific data
	const auto& shipLinkData_i = m_link.getShipLinkData(meeting.i);
	const double q = shipLinkData_i.getShipRate();
	const double mu = shipLinkData_i.getMu();
	const double sigma = shipLinkData_i.getSigma();

	// Aggregate results for each sublink
	// TODO multiplication with q and perhaps with causation factor can be moved up in the for-loop hierarchy
	driftCollisions = 0.0;
	ramCollisions = 0.0;
	for (auto& subLink : m_subLinks) {

		// Probability to be on this sulink
		double leftBoundary = subLink.getOffset() - 0.5 * m_subLinkWidth;
		double rightBoundary = leftBoundary + m_subLinkWidth;
		double pSubLink = sml::cdf_normalDistribution(rightBoundary, mu, sigma) - sml::cdf_normalDistribution(leftBoundary, mu, sigma);

		// Compute the drift exposure
		const auto& meetingDrift_i = m_meetingsDrift[meeting.i];
		for (size_t localObjectIndex = 0; localObjectIndex < meetingDrift_i.globalObjectIndex.size(); localObjectIndex++) {
			if (meetingDrift_i.globalObjectIndex[localObjectIndex] == meeting.j) {
				driftCollisions += pSubLink * q * meetingDrift_i.collisions[localObjectIndex];
			}
		}

		// Compute the ram exposure
		const auto& meetingRam_i = m_meetingsRam[meeting.i];
		for (size_t localObjectIndex = 0; localObjectIndex < meetingRam_i.globalObjectIndex.size(); localObjectIndex++) {
			if (meetingRam_i.globalObjectIndex[localObjectIndex] == meeting.j) {
				ramCollisions += pSubLink * q * meetingRam_i.collisions[localObjectIndex];
			}
		}
	}
}



void LinkObject::projectCoordinates(const std::vector<const sim::Object*>& objectList, std::vector<std::vector<sml::Vector2d>>& verticesObjects)
{
	for (const auto object : objectList) {
		std::vector<sml::Vector2d> vertices(object->getCoordinates().size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i] = sml::GeoUtils::projectToPlane(getOrigin(), object->getCoordinates()[i]);
		}
		verticesObjects.push_back(vertices);
	}
}



bool LinkObject::usePassingDistance(const sim::Object* object) const
{
	return m_model.getUsePassingDistance() && object->isClosed() && object->getArea() < GeneralModelData::passingDistanceobjectArea();
}
} // namespace sem

