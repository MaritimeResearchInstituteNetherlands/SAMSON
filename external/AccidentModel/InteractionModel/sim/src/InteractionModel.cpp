// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/InteractionModel.h>
#include <algorithm>
#include <cmath>

#include <sml/Point.h>
#include <sml/GeoUtils.h>
#include <sio/csv/csv_iarchive.h>
#include <sio/csv/csv_oarchive.h>

namespace sim
{

InteractionModel::InteractionModel(tdb::TrafficDataBase& trafficDataBase)
	: m_trafficDataBase(trafficDataBase)
	, m_areaModel(trafficDataBase.getAreaModel())
	, m_trafficModel(trafficDataBase.getTrafficModel())
	, m_exposureModel(dynamic_cast<sim::IExposureModel&>(*trafficDataBase.getExposureModel()))
	, m_exposuresDone(false)
	, m_eventsDone(false)
	, m_collisionsDone(false)
	, m_parallel(false)
{
	setup();
}



// Generates the full interaction model
void InteractionModel::setup()
{
	// Generic model setup
	m_exposureModel.setup(m_areaModel, m_trafficModel);

	// Create the model elements
	createModelLinks();
	createModelCells();
	createModelObjects();

	// Load the traffic, objects do not contain traffic
	loadTrafficOnLinks();
	loadTrafficOnCells();

	// Now that all data is attached to the model entities, finalise them (e.g. compute model specific values such as link width)
	finaliseModelEntities();

	// Generate the pair interactions (LinkLink, LinkCell, LinkObject, CellObject)
	createPairInteractions();

	// With pair interactions created, finalise the interaction (e.g. compute angles between links)
	finalisePairEntities();
}



void InteractionModel::reset()
{
	m_exposuresDone = false;
	m_eventsDone = false;
	m_collisionsDone = false;
}



void InteractionModel::computeExposures()
{
	computeExposures(m_links, "link");
	computeExposures(m_cells, "cell");
	computeExposures(m_linkLinks, "linklink");
	computeExposures(m_linkCells, "linkcell");
	computeExposures(m_linkObjects, "linkobject");
	computeExposures(m_cellObjects, "cellobject");
	m_exposuresDone = true;
}



void InteractionModel::computeEvents()
{
	if (!m_exposuresDone) {
		LogWarning("Compute or load exposures before computing events");
		return;
	}
	computeEvents(m_links, "link");
	computeEvents(m_cells, "cell");
	computeEvents(m_linkLinks, "linklink");
	computeEvents(m_linkCells, "linkcell");
	computeEvents(m_linkObjects, "linkobject");
	computeEvents(m_cellObjects, "cellobject");
	m_eventsDone = true;
}



void InteractionModel::computeCollisions()
{
	if (!m_eventsDone) {
		LogWarning("Compute or load events before computing collisions");
		return;
	}
	computeCollisions(m_links, "link");
	computeCollisions(m_cells, "cell");
	computeCollisions(m_linkLinks, "linklink");
	computeCollisions(m_linkCells, "linkcell");
	computeCollisions(m_linkObjects, "linkobject");
	computeCollisions(m_cellObjects, "cellobject");
	m_collisionsDone = true;
}



void InteractionModel::createModelLinks()
{
	const auto& links = m_areaModel.getLinks();
	const auto& wayPoints = m_areaModel.getWayPoints();
	const auto& vertices = m_areaModel.getVertices().getIndexedData();
	for (size_t index = 0; index < links.size(); index++) {
		const auto& link = links.get(index);
		auto modelLink = m_exposureModel.createLink(index, link, m_trafficDataBase);

		// Collect the link coordinates from the area database and store them
		size_t startVertex = wayPoints.get(link->getStart())->getVertexUID();
		size_t endVertex = wayPoints.get(link->getEnd())->getVertexUID();
		sml::Coordinate2D start = { vertices[startVertex]->getLatitude(), vertices[startVertex]->getLongitude() };
		sml::Coordinate2D end = { vertices[endVertex]->getLatitude(), vertices[endVertex]->getLongitude() };
		modelLink->addCoordinate(start);
		modelLink->addCoordinate(end);
		modelLink->setWaypointStart(wayPoints.get(link->getStart()));
		modelLink->setWaypointEnd(wayPoints.get(link->getEnd()));

		// Add the indicator link
		m_links.push_back(std::move(modelLink));
	}
}



void InteractionModel::createModelCells()
{
	const auto& cells = m_areaModel.getCells();
	for (size_t index = 0; index < cells.size(); index++) {
		const auto& cell = cells.get(index);

		auto modelCell = m_exposureModel.createCell(index, cell, m_trafficDataBase);

		// Collect the cell coordinates from the area database
		const auto& vertices = m_areaModel.getVertices();
		const auto& vertexIndices = cell->getVertices();
		for (size_t vertexIndex : vertexIndices) {
			const auto& vertex = vertices.get(vertexIndex);
			modelCell->addCoordinate({ vertex->getLatitude(), vertex->getLongitude() });
		}

		// Add the indicator cell
		m_cells.push_back(std::move(modelCell));
	}
}



void InteractionModel::createModelObjects()
{
	const auto& objects = m_areaModel.getObjects();
	for (size_t index = 0; index < objects.size(); index++) {
		const auto& object = objects.get(index);

		// Collect the cell coordinates from the area database
		std::vector<sml::Coordinate2D> coordinates;
		const auto& vertices = m_areaModel.getVertices();
		const auto& vertexIndices = object->getVertices();
		for (size_t vertexIndex : vertexIndices) {
			const auto& vertex = vertices.get(vertexIndex);
			coordinates.push_back({vertex->getLatitude(), vertex->getLongitude()});
		}

		auto modelObject = m_exposureModel.createObject(index, object, coordinates);



		// Add the indicator object
		m_objects.push_back(std::move(modelObject));
	}
}



void InteractionModel::loadTrafficOnLinks()
{
	// Note: Currently this is a crude loading strategy
	// The refined algorithm would look something like this
	// 1) Sort on linkID
	// 2) Generate location list (by scanning?)
	// 3) add in parallel each shipCat to each link

	// Traffic database input
	const tdb::IndexedLinks& links = m_areaModel.getLinks();
	const tdb::IndexedShipLinks ShipLink = m_trafficModel.getShipLinks();

	// Populate Links with traffic
	for (size_t index = 0; index < ShipLink.size(); index++) {
		const auto& item = ShipLink.get(index);
		size_t linkIndex = links.getIndex(item->getLinkUid());
		if (linkIndex == links.size()) {
			LogWarning(" Link " << item->getLinkUid() << " in shiplink " << item->getUID() << " can't be found. ");
			continue;
		}
		size_t shipLinkDataIndex = m_exposureModel.getShipLinkDataIndex(item->getShipLinkDataUid());
		size_t shipCategoryIndex = m_trafficModel.getShipCategories().getIndex(item->getShipCategoryUid());
		m_links[linkIndex]->addShip(index, shipLinkDataIndex, shipCategoryIndex);
		//std::cout << "Adding ship " << item->getShipUid() << " on link " << item->getLinkUid() << " with data " << item->getShipLinkDataUid()
		//	<< " resulting in indices {" << shipLinkDataIndex << "," << shipCategoryIndex << "}"
		//	<< std::endl;
	}


}



void InteractionModel::loadTrafficOnCells()
{
	const auto& cells = m_areaModel.getCells();
	const auto& ShipCell = m_trafficModel.getShipCells();
	// Populate Cells with traffic
	for (size_t index = 0; index < ShipCell.size(); index++) {
		auto item = ShipCell.get(index);
		size_t cellIndex = cells.getIndex(item->getCellUid());
		if (cellIndex == cells.size()) {
			LogWarning("Cell " << item->getCellUid() << " in shipcell " << item->getUID() << " can't be found. ");
			continue;
		}
		size_t shipCellDataIndex = m_exposureModel.getShipCellDataIndex(item->getShipCellDataUid());
		size_t shipCategoryIndex = m_trafficModel.getShipCategories().getIndex(item->getShipCategoryUid());
		m_cells[cellIndex]->addShip(index, shipCellDataIndex, shipCategoryIndex);
		// TODO the output of the data should not output shipcat, but shiplink and shipcell
	}
}



void InteractionModel::finaliseModelEntities()
{
	// Finalise the interaction entities
	finaliseSetup(m_links);	// (e.g. compute the width, based on model specific traffic on the link)
	finaliseSetup(m_cells);
}



void InteractionModel::finalisePairEntities()
{
	finaliseSetup(m_linkLinks);
	finaliseSetup(m_linkCells);
	finaliseSetup(m_linkObjects);
	finaliseSetup(m_cellObjects);
}



void InteractionModel::createPairInteractions()
{
	// TODO If you want to do this parallel for_each can perhaps better be done on the outerloop that on the interloop, such that the interloop tasks are not too small
	// LinkLink: A link may only interact in pairs. Only loop over the active links
	for (auto linkA = m_links.begin(); linkA != m_links.end(); linkA++) {
		std::for_each(std::execution::seq, std::next(linkA), m_links.end(), [&](const std::unique_ptr<sim::Link>& linkB)
			{
				if (m_exposureModel.isLinkLinkPair((**linkA), *linkB)) {
					auto indicatorLinkLink = m_exposureModel.createLinkLink(**linkA, *linkB, m_trafficDataBase);
					LogVerbose("Creating linklink interaction between link " << (*linkA)->getUID() << " and " << linkB->getUID());
					double dot = linkA->get()->getDirection() * linkB->getDirection();
					double angle;
					if (dot < -1) {
						angle = std::numbers::pi;
					} else {
						angle = std::acos(dot);
					}
					indicatorLinkLink->setAngle(angle);

					// Store
					m_linkLinks.push_back(std::move(indicatorLinkLink));
				}
			}
		);
	}
	
	// LinkCell
	for (const auto& link : m_links) {
		for (const auto& cell : m_cells) {
			if (m_exposureModel.isLinkCellPair(*link.get(), *cell.get())) {
				LogVerbose("Creating linkcell interaction with link " << link->getUID() << " and cell " << cell->getUID());
				m_linkCells.push_back(std::move(m_exposureModel.createLinkCell(*link, *cell, m_trafficDataBase)));
			}
		}
	}
	
	// LinkObject
	for (auto link = m_links.begin(); link != m_links.end(); link++) {
		// Get a list of interacting objects
		std::vector<const sim::Object*> interactingObjects;
		for (auto object = m_objects.begin(); object != m_objects.end(); object++) {
			if (m_exposureModel.isLinkObjectPair((*link)->getIndex(), (*object)->getIndex())) {
				interactingObjects.push_back((*object).get());
			}
		}

		// Add LinkObject interaction if required
		if (interactingObjects.size() > 0.0) {
			// Grab the relevant weather data
			// Note to discuss + for in the documentation: The origin for the weather is defined at half the link length
			const sml::Coordinate2D start = (*link)->getCoordinates()[0];
			const sml::Coordinate2D end = (*link)->getCoordinates()[1];
			const sml::Coordinate2D origin = { start.lat + 0.5 * (end.lat - start.lat), start.lon + 0.5 * (end.lon - start.lon) };
			tdb::WeatherstationScheme::UidType wsUid = findClosestWeatherStation(origin);
			const auto& wsScheme = m_areaModel.getWeatherstations().get(wsUid);
			LogVerbose("Creating linkObject with link " << (*link)->getUID());
			auto linkObject = m_exposureModel.createLinkObject(**link, m_trafficDataBase, *wsScheme);
			for (auto object : interactingObjects) {
				LogVerbose("Adding object " << object->getUID() << " to linkObject with link " << (*link)->getUID());
				linkObject->addObject(object);
			}
			m_linkObjects.push_back(std::move(linkObject));
		}
	}
	
	 // CellObject
	for (auto cell = m_cells.begin(); cell != m_cells.end(); cell++) {
		// Get a list of interacting objects
		std::vector<const sim::Object*> interactingObjects;
		for (auto object = m_objects.begin(); object != m_objects.end(); object++) {
			if (m_exposureModel.isCellObjectPair((*cell)->getIndex(), (*object)->getIndex())) {
				interactingObjects.push_back((*object).get());
			}
		}

		// Add CellObject interaction if required
		if (interactingObjects.size() > 0.0) {
			// Grab the relevant weather data for the drift model
			const sml::Coordinate2D origin = (*cell)->getOrigin();
			tdb::WeatherstationScheme::UidType wsUid = findClosestWeatherStation(origin);
			const auto& wsScheme = m_areaModel.getWeatherstations().get(wsUid);

			// Create the interaction
			LogVerbose("Creating cellObject with cell " << (*cell)->getUID());
			auto cellObject = m_exposureModel.createCellObject(**cell, m_trafficDataBase, *wsScheme);
			for (auto object : interactingObjects) {
				LogVerbose("Adding object " << object->getUID() << " to linkObject with cell " << (*cell)->getUID());
				cellObject->addObject(object);
			}
			m_cellObjects.push_back(std::move(cellObject));
		}
	}
}



std::string InteractionModel::findClosestWeatherStation(const sml::Coordinate2D& origin)
{
	if (m_areaModel.getWeatherstations().size() == 0) {
		LogWarning("No weather stations found in the database");
	}

	double minDistance = DBL_MAX;
	tdb::WeatherstationScheme::UidType wsUid;
	for (const auto ws : m_areaModel.getWeatherstations().getIndexedData()) {
		const auto& vertex = m_areaModel.getVertices().getIndexedData()[ws->getVertexUID()];
		double distance = sml::GeoUtils::distance(origin, {vertex->getLatitude(), vertex->getLongitude()});
		if (distance < minDistance) {
			minDistance = distance;
			wsUid = ws->getUID();
		}
	}
	return wsUid;
}

} // namespace sim
