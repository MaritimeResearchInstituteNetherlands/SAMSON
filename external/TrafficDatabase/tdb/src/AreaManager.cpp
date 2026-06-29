// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/AreaManager.h>

namespace tdb {
AreaManager::AreaManager(AreaModel& areaModel)
	: m_areaModel(areaModel)
{}



bool AreaManager::command(std::stringstream& input)
{
	std::string command;
	input >> command;
	std::string line;
	std::getline(input, line);
	if (command == "add") {;
		m_add.push_back(line);
	}
	else if (command == "modify") {
		m_modify.push_back(line);
	} 
	else if (command == "remove") {
		m_remove.push_back(line);
	}
	else {
		LogWarning("AreaManager: unknown command " << command);
		// TODO improve this warning by listing the possible commands
		return false;
	}
	return true;
}



bool AreaManager::apply() 
{
	bool result = true;
	// Adding only adds to the database, it should not break any existing data
	LogInfo("Applying modifications");
	for (auto& command : m_add) {
		std::stringstream input(command);
		if (!add(input)) {
			result = false;
		}
	}
	m_add.clear();

	// Modify only adds to existing entities
	for (auto& command : m_modify) {
		std::stringstream input(command);
		if (!modify(input)) {
			result = false;
		}
	}
	m_modify.clear();
	
	// Removing changes the vertices of all entities and require an indexing step
	// TODO this can be done more efficient
	for (auto& command : m_remove) {
		std::stringstream input(command);
		if (!remove(input)) {
			result = false;
		}
	}
	m_remove.clear();

	return result;
}



bool AreaManager::add(std::stringstream& input)
{
	// TODO This serialisation should be done smarter, remove the boost archive and implement a streaming type save/load
	std::string type;
	input >> type;
	if (type == "grid") {
		return addGrid(input);
	} 
	else if (type == "waypoint") {
		return addWaypoint(input);
	} 
	else if (type == "link") {
		return addLink(input);
	}
	else {
		LogWarning("AreaManager: Adding type " << type << " is not supported.");
		// TODO list the supported values
		return false;
	}
}



bool AreaManager::modify(std::stringstream& input)
{
	std::string type;
	input >> type;
	if (type == "vertex") {
		return modifyVertex(input);
	}
	else {
		LogWarning("AreaManager: modify for type " << type << " is not supported.");
		// TODO improve by listing the supporting .. 
		return false;
	}
}



bool AreaManager::remove(std::stringstream& input)
{
	std::string type;
	input >> type;
	std::string uid;
	input >> uid;
	//std::vector<std::string> uids;
	//while (input >> uid) {
	//	uids.push_back(uid);
	//}
	if (type == "grid") {
		return removeGrid(uid);
	}
	//else if (type == "cell") {
	//	return removeCell()
	//}
	else {
		LogWarning("AreaManager: removing type " << type << " is not supported");
		// TODO improve by listing the supported values
		return false;
	}
}



bool AreaManager::addGrid(std::stringstream& input)
{
	// Extract specific input
	std::string uid;
	sml::Coordinate2D bottomLeft;
	sml::Coordinate2D topRight;
	int nlat;
	int nlon;
	input >> uid;
	input >> bottomLeft.lat;
	input >> bottomLeft.lon;
	input >> topRight.lat;
	input >> topRight.lon;
	input >> nlat;
	input >> nlon;

	AreaDataBase& areaDataBase = m_areaModel.getAreaDataBase();

	// Check that gridUID does not exist
	for (auto cell : areaDataBase.cellDataBase) {
		if (cell->getUIDGrid() == uid) {
			LogError("gridUID " << uid << " already exists in the area database.");
			return false;
		}
	}

	VertexScheme::UidType startVertex = areaDataBase.vertexDataBase.size();
	VertexScheme::UidType currentVertex = startVertex;

	const double dlat = (topRight.lat - bottomLeft.lat) / (nlat - 1);
	const double dlon = (topRight.lon - bottomLeft.lon) / (nlon - 1);

	if (dlat <= 0 || dlon <= 0) {
		LogError("Coordinates of grid do not represent the bottomleft and topright point");
	}
	
	// Add vertices
	auto& vertexDataBase = areaDataBase.vertexDataBase;
	for (int j = 0; j < nlat; j++) {
		for (int i = 0; i < nlon; i++) {
			vertexDataBase.push_back(std::make_shared<tdb::VertexScheme>(currentVertex, bottomLeft.lat + j * dlat, bottomLeft.lon + i * dlon));
			currentVertex++;
		}
	}

	// Add cells
	auto& cellDataBase = areaDataBase.cellDataBase;
	int cellIndex = 0;
	for (int i = 0; i < nlon - 1; i++) {
		for (int j = 0; j < nlat - 1; j++) {
			// Otabin vertices
			tdb::VertexScheme::UidType bottomLeft = startVertex + i + j * nlon;
			tdb::VertexScheme::UidType bottomRight = bottomLeft + 1;
			tdb::VertexScheme::UidType topLeft = startVertex + i + (j + 1) * nlon;
			tdb::VertexScheme::UidType topRight = topLeft + 1;

			// Add cell
			std::string uidCell = "C" + std::to_string(cellIndex) + "_" + uid;
			std::vector<tdb::VertexScheme::UidType> vertices = { bottomLeft, bottomRight, topRight, topLeft, bottomLeft };
			cellDataBase.push_back(std::make_shared<tdb::CellScheme>(uidCell, vertices, uid, ""));
			cellIndex++;
		}
	}

	// Database is adapted, re-index it and validate
	m_areaModel.indexDataBase();
	if (!m_areaModel.validate()) {
		return false;
	}

	return true;
}



bool AreaManager::addWaypoint(std::stringstream& input)
{
	// Extract data
	std::string uid;
	sml::Coordinate2D coord;
	std::string description;
	double rangeOfInfluence;
	input >> uid;
	input >> coord.lat;
	input >> coord.lon;
	input >> description;
	input >> rangeOfInfluence;

	AreaDataBase& areaDataBase = m_areaModel.getAreaDataBase();
	auto& waypointDataBase = areaDataBase.waypointDataBase;
	auto& vertexDataBase = areaDataBase.vertexDataBase;

	VertexScheme::UidType vertexUid = areaDataBase.vertexDataBase.size();
	vertexDataBase.push_back(std::make_shared<tdb::VertexScheme>(vertexUid, coord.lat, coord.lon));
	waypointDataBase.push_back(std::make_shared<tdb::WaypointScheme>(uid, vertexUid, rangeOfInfluence, description));

	// Database is adapted, re-index it and validate
	m_areaModel.indexDataBase();
	if (!m_areaModel.validate()) {
		return false;
	}

	return true;
}



bool AreaManager::addLink(std::stringstream& input)
{
	// Extract data
	std::string uid;
	tdb::WaypointScheme::UidType start;
	tdb::WaypointScheme::UidType end;
	std::string description;
	input >> uid;
	input >> start;
	input >> end;
	input >> description;

	AreaDataBase& areaDataBase = m_areaModel.getAreaDataBase();
	auto& waypointDataBase = areaDataBase.waypointDataBase;
	auto& linkDataBase = areaDataBase.linkDataBase;

	linkDataBase.push_back(std::make_shared<tdb::LinkScheme>(uid, start, end, description));

	// Database is adapted, re-index it and validate
	m_areaModel.indexDataBase();
	if (!m_areaModel.validate()) {
		return false;
	}

	return true;
}



//bool AreaManager::addCell(const std::string& uid)
//{
//	AreaDataBase& areaDataBase = m_areaModel.getAreaDataBase();
//	auto& cellDataBase = areaDataBase.linkDataBase;
//
//	cellDataBase.push_back(std::make_shared<tdb::CellScheme>(uid, vertices, uid, ""));
//}



bool AreaManager::modifyVertex(std::stringstream& input)
{
	// Get input data
	int uid;
	double lat;
	double lon;
	input >> uid;
	input >> lat;
	input >> lon;
	auto vertex = m_areaModel.getVertices().get(uid);
	vertex->setLattitude(lat);
	vertex->setLongitude(lon);
	return true;
}



bool AreaManager::removeGrid(const std::string& gridUID)
{
	AreaDataBase& areaDataBase = m_areaModel.getAreaDataBase();

	// Check that gridUID does exist
	bool exists = false;
	for (auto cell : areaDataBase.cellDataBase) {
		if (cell->getUIDGrid() == gridUID) {
			exists = true;
			break;
		}
	}
	if (!exists) {
		LogError("Error: trying to remove non-existing grid " << gridUID);
		return false;
	}

	// Count how often the vertices are used
	std::vector<int> indexMapping(areaDataBase.vertexDataBase.size(), 0);
	mapIndices(areaDataBase, indexMapping, gridUID);	// Perhaps move to a kind of "filter", not just gridUID then removings can be done simultaneously

	// Create new vertex database
	DataTable<VertexScheme> vertexDataBase;
	for (auto vertex : areaDataBase.vertexDataBase) {
		if (indexMapping[vertex->getIndex()] >= 0) {
			vertexDataBase.push_back(std::make_shared<tdb::VertexScheme>(*vertex));
		}
	}
	std::swap(areaDataBase.vertexDataBase, vertexDataBase);

	// Create new cell dataBase
	DataTable<CellScheme> cellDataBase;
	for (auto cell : areaDataBase.cellDataBase) {
		if (cell->getUIDGrid() != gridUID) {
			cellDataBase.push_back(std::make_shared<tdb::CellScheme>(*cell));
		}
	}
	std::swap(areaDataBase.cellDataBase, cellDataBase);

	// Update and verify the area model
	updateAreaIndices(areaDataBase, indexMapping);
	m_areaModel.indexDataBase();
	if (!m_areaModel.validate()) {
		return false;
	}

	return true;
}



//bool AreaManager::remove(const GroupedUids& groupedUids)
//{
//	AreaDataBase& areaDataBase = m_areaModel.getAreaDataBase();
//
//	std::vector<int> indexMapping(areaDataBase.vertexDataBase.size(), 0);
//	//mapIndices(areaDataBase, indexMapping, gridUID);	// Perhaps move to a kind of "filter", not just gridUID then removings can be done simultaneously
//}



//void AreaManager::mapIndices(const AreaDataBase& areaDataBase, std::vector<int>& indexMapping, const GroupedUids& groupedUids)
//{
//	// Count how often a vertex is actively used
//
//	for (auto waypoint : areaDataBase.waypointDataBase) {
//		if (std::find(groupedUids.waypointUids.begin(), groupedUids.waypointUids.end(), waypoint->getUID()) != groupedUids.waypointUids.end()) {
//			continue;
//		}
//		indexMapping[waypoint->getVertexUID()]++;
//	}
//	for (auto cell : areaDataBase.cellDataBase) {
//		int value = 0;
//		cell->getUIDGrid() == gridUID ? value = 0 : value = 1;
//		for (auto vertex : cell->getVertices()) {
//			indexMapping[vertex] += value; // If vertex is only used by the gruidUID, the vertex ends up being negative, flagging it for removal
//		}
//	}
//	for (auto object : areaDataBase.objectDataBase) {
//		for (auto vertex : object->getVertices()) {
//			indexMapping[vertex]++;
//		}
//	}
//	for (auto weatherstation : areaDataBase.weatherstationDataBase) {
//		indexMapping[weatherstation->getVertexUID()]++;
//	}
//	for (auto ertv : areaDataBase.ertvDataBase) {
//		indexMapping[ertv->getVertexUID()]++;
//	}
//}



void AreaManager::mapIndices(const AreaDataBase& areaDataBase, std::vector<int>& indexMapping, const std::string& gridUID)
{
	// Count how often a vertex is actively used
	for (auto waypoint : areaDataBase.waypointDataBase) {
		indexMapping[waypoint->getVertexUID()]++;
	}
	for (auto cell : areaDataBase.cellDataBase) {
		int value = 0;
		cell->getUIDGrid() == gridUID ? value = 0 : value = 1;
		for (auto vertex : cell->getVertices()) {
			indexMapping[vertex] += value; // If vertex is only used by the gruidUID, the vertex ends up being negative, flagging it for removal
		}
	}
	for (auto object : areaDataBase.objectDataBase) {
		for (auto vertex : object->getVertices()) {
			indexMapping[vertex]++;
		}
	}
	for (auto weatherstation : areaDataBase.weatherstationDataBase) {
		indexMapping[weatherstation->getVertexUID()]++;
	}
	for (auto ertv : areaDataBase.ertvDataBase) {
		indexMapping[ertv->getVertexUID()]++;
	}


	//std::cout << "Index counting: " << std::endl;
	//for (auto index : indexMapping) {
	//	std::cout << index << std::endl;
	//}


	// Now give the indices the correct new index
	int currentIndex = 0;
	for (int& index : indexMapping) {
		if (index > 0) {
			index = currentIndex;
			currentIndex++;
		}
		else {
			index = -1;
		}
	}

	//std::cout << "Index mapping: " << std::endl;
	//for (auto index : indexMapping) {
	//	std::cout << index << std::endl;
	//}
}



void AreaManager::updateAreaIndices(const AreaDataBase& areaDataBase, std::vector<int>& indexMap)
{
	for (auto vertex : areaDataBase.vertexDataBase) {
		vertex->setIndex(indexMap[vertex->getIndex()]);
	}
	for (auto waypoint : areaDataBase.waypointDataBase) {
		waypoint->setVertexUID(indexMap[waypoint->getVertexUID()]);
	}
	for (auto cell : areaDataBase.cellDataBase) {
		std::vector<VertexScheme::UidType> vertices;
		for (auto vertex : cell->getVertices()) {
			vertices.push_back(indexMap[vertex]);
		}
		cell->setVertices(vertices);
	}
	for (auto object : areaDataBase.objectDataBase) {
		std::vector<VertexScheme::UidType> vertices;
		for (auto vertex : object->getVertices()) {
			vertices.push_back(indexMap[vertex]);
		}
		object->setVertices(vertices);
	}
	for (auto weatherstation : areaDataBase.weatherstationDataBase) {
		weatherstation->setVertexUID(indexMap[weatherstation->getVertexUID()]);
	}
	for (auto ertv : areaDataBase.ertvDataBase) {
		ertv->setVertexUID(indexMap[ertv->getVertexUID()]);
	}
}

} // namespace tdb
