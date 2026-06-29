// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/CellObject.h>
#include <sem/Cell.h>
#include <sem/SubCellObject.h>
#include <sml/GeoUtils.h>

namespace sem
{

CellObject::CellObject(const sim::Cell& cell, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase, const tdb::WeatherstationScheme& wss)
	: base(cell)
	, m_cell(dynamic_cast<const sem::Cell&>(cell))
	, m_model(model)
	, m_driftModels(model.getDriftModels(wss))
{
}



void CellObject::setup()
{
	bool useShielding = false;
	// Group objects based on shielding property
	// TODO this is not yet fully implemented
	if (useShielding) {
		for (auto object : getObjects()) {
			m_shieldingObjects.push_back(object);
		}
	}
	else {
		for (auto object : getObjects()) {
			m_nonShieldingObjects.push_back(object);
		}
	}

	// Project all relevant object coordinates to vertices for the given origin of this LinkObject
	projectCoordinates(m_shieldingObjects, m_verticesShieldingObjects);
	projectCoordinates(m_nonShieldingObjects, m_verticesNonShieldingObjects);
	
	discretise();

	// Generate the meetings
	m_meetingsDrift.resize(getCell().getNumShips());
	for (auto& meeting : m_meetingsDrift) {
		meeting.exposures.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.collisions.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.objectIndex.resize(m_nonShieldingObjects.size());
	}
	m_meetingsRam.resize(getCell().getNumShips());
	for (auto& meeting : m_meetingsRam) {
		meeting.exposures.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.collisions.resize(m_nonShieldingObjects.size(), 0.0);
		meeting.objectIndex.resize(m_nonShieldingObjects.size());
	}

	base::setup();
	setNumShips(getCell().getNumShips());
	setNumObjects(m_nonShieldingObjects.size());
}



void CellObject::prepareExposureComputation()
{
	for (auto& subCell : m_subCells) {
		subCell.setup();
	}

	// Compute the detailed exposure data for each ship
	LogInfo("Computing CellObject detailed exposures for cell " << getCell().getUID());
	for (size_t i = 0; i < m_cell.getNumShips(); i++) {
		const auto& shipCategory = m_cell.getShipCategory(i);
		const auto& shipCellData = m_cell.getShipCellData(i);
		
		const double elevationMin = -shipCategory.getDraft();
		const double elevationMax = elevationMin + shipCategory.getHeight();
		const sem::DriftModel& shipDriftModel = m_driftModels[m_cell.getShipCategoryIndex(i)];
		
		// Perform the detailed computation
		for (auto& subCell : m_subCells) {		
			subCell.computeExposuresDrift(elevationMin, elevationMax, shipDriftModel, m_meetingsDrift[i]);
			subCell.computeExposuresRam(elevationMin, elevationMax, shipCellData.getDirection(), m_meetingsRam[i]);
		}
	}

	for (auto& subCell : m_subCells) {
		subCell.cleanUp();
	}
}



void CellObject::prepareEventComputation()
{
}



void CellObject::prepareCollisionComputation()
{
	for (auto& subCell : m_subCells) {
		subCell.setup();
	}

	// Compute the detailed exposure data for each ship
	LogInfo("Computing CellObject detailed measures for cell " << getCell().getUID());
	for (auto& subCell : m_subCells) {
		for (size_t i = 0; i < m_cell.getNumShips(); i++) {
			const auto& shipCategory = m_cell.getShipCategory(i);
			const auto& shipCellData = m_cell.getShipCellData(i);
			const auto& shipDriftModel = m_driftModels[m_cell.getShipCategoryIndex(i)];

			const double direction = shipCellData.getDirection();
			const double length = shipCategory.getLength();
			const double elevationMin = shipCategory.getDraft();
			const double elevationMax = elevationMin + shipCategory.getHeight();

			// Perform the detailed computation!
			//subCell.computeCollisions(i, -draft, -draft + height, direction, length, shipDriftModel);
			subCell.computeCollisionsDrift(elevationMin, elevationMax, shipDriftModel, m_meetingsDrift[i]);
			subCell.computeCollisionsRam(elevationMin, elevationMax, direction, length, m_meetingsRam[i]);
		}
	}

	for (auto& subCell : m_subCells) {
		subCell.cleanUp();
	}
}



void CellObject::computeExposure(const sim::ShipObjectMeeting& meeting, double& driftExposure, double& ramExposure)
{
	// Aggregate detailed results by summing up all contributions for a given ship and object index 
	driftExposure = 0.0;
	ramExposure = 0.0;
	
	// Ship specific data
	const auto& shipCellData = m_cell.getShipCellData(meeting.i);
	const auto& shipCategoryData = m_cell.getShipCategory(meeting.i);
	const double u = getShipSpeed(shipCellData.getSpeed(), shipCategoryData);
	const double density = shipCellData.getDensity();

	// Compute the drift and ram exposure for the specific ship i and object j combination
	const auto& meetingDrift = m_meetingsDrift[meeting.i];
	for (size_t localObjectIndex = 0; localObjectIndex < meetingDrift.objectIndex.size(); localObjectIndex++) {
		if (meetingDrift.objectIndex[localObjectIndex] == meeting.j) {
			driftExposure += meetingDrift.exposures[localObjectIndex];
		}
	}
	const auto& meetingRam = m_meetingsRam[meeting.i];
	for (size_t localObjectIndex = 0; localObjectIndex < meetingRam.objectIndex.size(); localObjectIndex++) {
		if (meetingRam.objectIndex[localObjectIndex] == meeting.j) {
			ramExposure += meetingRam.exposures[localObjectIndex];
		}
	}

	driftExposure *= density * u;
	ramExposure *= density * u;
}



void CellObject::computeEvents(const sim::ShipObjectMeeting& meeting, double& driftEvents, double& ramEvents)
{
	// Grab databases
	const auto& shipCategory = m_cell.getShipCategory(meeting.i);
	const auto& causationFactors = m_model.getCausationModel().getCausationFactors();

	// Get the causation factor
	const size_t index = causationFactors.getIndex(shipCategory.getUID());
	if (index == causationFactors.size()) {
		LogError("Ship class " << shipCategory.getUID() << " does not have an entry in the causation factor database.");
	}
	const auto& entry = causationFactors.get(index);
	const double driftFactor = entry->getDriftFactor();
	const double ramFactor = entry->getRamFactor();

	// Cache in meeting data for future measure computations
	for (auto& meeting : m_meetingsDrift) {
		meeting.causationFactor = driftFactor;
	}
	for (auto& meeting : m_meetingsRam) {
		meeting.causationFactor = ramFactor;
	}

	//for (auto& subCell : m_subCells) {
	//	subCell.computeEvents(meeting.i, driftFactor, ramFactor);
	//}

	// Apply to aggregated data
	// Note: since this factor is not -inside- the for-loops, the aggregated results are identical to the results when aggregating the details events
	driftEvents = driftFactor * meeting.driftExposure;
	ramEvents = ramFactor * meeting.ramExposure;
}



void CellObject::computeCollisions(const sim::ShipObjectMeeting& meeting, double& driftCollisions, double& ramCollisions)
{
	// Ship specific data
	const auto& shipCellData = m_cell.getShipCellData(meeting.i);
	const auto& shipCategoryData = m_cell.getShipCategory(meeting.i);
	const double u = getShipSpeed(shipCellData.getSpeed(), shipCategoryData);
	const double density = shipCellData.getDensity();

	// Aggregate results for each subcell
	driftCollisions = 0.0;
	ramCollisions = 0.0;

	// Compute the drift and ram exposure for the specific ship i and object j combination
	const auto& meetingDrift = m_meetingsDrift[meeting.i];
	for (size_t localObjectIndex = 0; localObjectIndex < meetingDrift.objectIndex.size(); localObjectIndex++) {
		if (meetingDrift.objectIndex[localObjectIndex] == meeting.j) {
			driftCollisions += meetingDrift.collisions[localObjectIndex];
		}
	}

	// Compute the ram exposure
	const auto& meetingRam = m_meetingsRam[meeting.i];
	for (size_t localObjectIndex = 0; localObjectIndex < meetingRam.objectIndex.size(); localObjectIndex++) {
		if (meetingRam.objectIndex[localObjectIndex] == meeting.j) {
			ramCollisions += meetingRam.collisions[localObjectIndex];
		}
	}
	driftCollisions *= density * u;
	ramCollisions *= density * u;
}



void CellObject::projectCoordinates(const std::vector<const sim::Object*>& objectList, std::vector<std::vector<sml::Vector2d>>& verticesObjects)
{
	for (const auto object : objectList) {
		std::vector<sml::Vector2d> vertices(object->getCoordinates().size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i] = sml::GeoUtils::projectToPlane(getOrigin(), object->getCoordinates()[i]);
		}
		verticesObjects.push_back(vertices);
	}
}



void CellObject::discretise()
{
	double totalArea = 0.0;
	const auto& polygon = m_cell.getPolygon();

	// Generate cartesian discretisation
	auto [xMin, xMax, yMin, yMax] = polygon.getBoundingBox();
	size_t numCellsX = std::ceil((xMax - xMin) / GeneralModelData::maxSubCellGridSize());
	size_t numCellsY = std::ceil((yMax - yMin) / GeneralModelData::maxSubCellGridSize());

	// Cell is within a subcell
	if (numCellsX == 1 && numCellsY == 1) {
		auto [centroid, area] = polygon.getCentroidAndArea();
		m_subCells.push_back(SubCellObject(centroid, area, *this));
		totalArea += area;
	}

	double dx = (xMax - xMin) / numCellsX;
	double dy = (yMax - yMin) / numCellsY;

	// Clip polygon in sub polygons
	for (size_t i = 0; i < numCellsX; i++) {
		for (size_t j = 0; j < numCellsY; j++) {
			// Bounds of the local cell
			double x0 = xMin + dx * i;
			double x1 = x0 + dx;
			double y0 = yMin + dy * j;
			double y1 = y0 + dy;

			// Create local polygon
			sml::Polygon localCell;
			localCell.addVertex({x0, y0});
			localCell.addVertex({x1, y0});
			localCell.addVertex({x1, y1});
			localCell.addVertex({x0, y1});

			// Make intersection with cell
			std::vector<sml::Polygon> intersections = localCell.getIntersectionWith(polygon);

			// Add result
			for (const auto& intersection : intersections) {
				auto [centroid, area] = intersection.getCentroidAndArea();
				m_subCells.push_back(SubCellObject(centroid, area, *this));
				totalArea += area;
			}
		}
	}

	if (std::abs(1.0 - totalArea / polygon.getArea()) > 1e-3) {
		LogError("Internal error: mismatch in cell discretisation of CellObject interaction with cell " << m_cell.getUID() << ". Polygon area = " << polygon.getArea() << " and total subCellArea = " << totalArea);
	}
}



bool CellObject::usePassingDistance(const sim::Object* object) const
{
	return m_model.getUsePassingDistance() && object->isClosed() && object->getArea() < GeneralModelData::passingDistanceobjectArea();
}
} // namespace sem
