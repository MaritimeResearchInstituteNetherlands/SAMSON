// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/AreaModel.h>
#include <set>
#include <algorithm>
#include <numbers>

namespace tdb {

AreaModel::AreaModel()
{
}



bool AreaModel::loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames)
{
	// Load database schemes
	loadTable(path, filenames, "vertices", m_areaDataBase.vertexDataBase);
	loadTable(path, filenames, "waypoints", m_areaDataBase.waypointDataBase);
	loadTable(path, filenames, "links", m_areaDataBase.linkDataBase);
	loadTable(path, filenames, "cells", m_areaDataBase.cellDataBase);
	loadTable(path, filenames, "objects", m_areaDataBase.objectDataBase);
	loadTable(path, filenames, "zones", m_areaDataBase.zoneDataBase);
	loadTable(path, filenames, "weatherstations", m_areaDataBase.weatherstationDataBase);
	loadTable(path, filenames, "ertvs", m_areaDataBase.ertvDataBase);
	loadTable(path, filenames, "windstrength", m_areaDataBase.windstrengthDataBase);
	loadTable(path, filenames, "winddirection", m_areaDataBase.winddirectionDataBase);
	loadTable(path, filenames, "current", m_areaDataBase.currentDataBase);
	loadTable(path, filenames, "tide", m_areaDataBase.tideDataBase);
	// don't forget to update the help() if adding something here

	indexDataBase();

	// Validate the database
	return validate();
}



// Note, this is hard coded, if I add a field in the loadModel it is not automatically updated here. Which might not be ideal
void AreaModel::help()
{
	std::string header;
	std::ofstream dummy;
	boost::archive::csv_oarchive oa(dummy);

	LogInfo("AreaModel contains the following tables:");
	header = oa.generateHeader(VertexScheme());
	LogInfo("\t vertices:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(WaypointScheme());
	LogInfo("\t waypoints:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(LinkScheme());
	LogInfo("\t links:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(CellScheme());
	LogInfo("\t cells:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(ObjectScheme());
	LogInfo("\t objects:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(ZoneScheme());
	LogInfo("\t zones:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(WeatherstationScheme());
	LogInfo("\t weatherstations:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(ErtvScheme());
	LogInfo("\t ertvs:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(WindstrengthScheme());
	LogInfo("\t windstrength:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(WinddirectionScheme());
	LogInfo("\t winddirection:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(CurrentScheme());
	LogInfo("\t current:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(TideScheme());
	LogInfo("\t tide:");
	LogInfo("\t\t " << header);
}



void AreaModel::indexDataBase()
{
	m_vertices.clear();
	m_waypoints.clear();
	m_links.clear();
	m_cells.clear();
	m_objects.clear();
	m_zones.clear();
	m_weatherstations.clear();
	m_windStrengthTables.clear();
	m_windDirectionTables.clear();
	m_currentTables.clear();
	m_tideTables.clear();
	m_ertvs.clear();

	indexTable(m_areaDataBase.vertexDataBase, m_vertices);
	indexTable(m_areaDataBase.waypointDataBase, m_waypoints);
	indexTable(m_areaDataBase.linkDataBase, m_links);
	indexTable(m_areaDataBase.cellDataBase, m_cells);
	indexTable(m_areaDataBase.objectDataBase, m_objects);
	indexTable(m_areaDataBase.zoneDataBase, m_zones);
	indexTable(m_areaDataBase.weatherstationDataBase, m_weatherstations);
	indexTable(m_areaDataBase.ertvDataBase, m_ertvs);

	// Copy into associated table
	for (const auto& entry : m_areaDataBase.windstrengthDataBase) {
		m_windStrengthTables[entry->getTableUid()].push_back(entry);
	}
	// Sort on beaufort scale
	for (auto& table : m_windStrengthTables) {
		std::sort(table.second.begin(), table.second.end(), [](Data<WindstrengthScheme> a, Data<WindstrengthScheme> b) { return b->getScale() > a->getScale(); });
	}

	// Copy into associated table
	for (const auto& entry : m_areaDataBase.winddirectionDataBase) {
		m_windDirectionTables[entry->getTableUid()].push_back(entry);
	}
	// Sort on angle
	for (auto& table : m_windDirectionTables) {
		std::sort(table.second.begin(), table.second.end(), [](Data<WinddirectionScheme> a, Data<WinddirectionScheme> b) { return b->getAngle() > a->getAngle(); });
	}
	
	// Copy into associated table
	for (const auto& entry : m_areaDataBase.currentDataBase) {
		m_currentTables[entry->getTableUid()].push_back(entry);
	}
	// No need for sorting

	// Copy into associated table
	for (const auto& entry : m_areaDataBase.tideDataBase) {
		m_tideTables[entry->getTableUid()].push_back(entry);
	}
	// Sort on amplitude
	for (auto& table : m_tideTables) {
		std::sort(table.second.begin(), table.second.end(), [](Data<TideScheme> a, Data<TideScheme> b) { return b->getAmplitude() > a->getAmplitude(); });
	}
}



bool AreaModel::validate()
{
	bool valid = true;
	// Vertices, make sure that the uid is the same as the vector location
	const auto& indexedVertexData = m_vertices.getIndexedData();
	for (size_t index = 0; index < indexedVertexData.size(); index++) {
		const auto& vertex = indexedVertexData[index];
		if (vertex->getIndex() != index) {
			LogError("Vertex " << vertex->getUID() << " should have an UID of " << index);
			valid = false;
		}
	}
	

	// Waypoints
	size_t numWaypoints = m_waypoints.size(); // Termination value
	for (const auto& waypoint : m_waypoints.getIndexedData()) {
		const VertexScheme::UidType vertexUID = waypoint->getVertexUID();
		if (m_vertices.get(vertexUID) == nullptr) {
			LogError("Waypoint " << waypoint->getUID() << " has an incorrect vertex " << vertexUID);
			valid = false;
		}
	}

	// Links
	for (const auto& link : m_links.getIndexedData()) {
		const WaypointScheme::UidType start = link->getStart();
		const WaypointScheme::UidType end = link->getEnd();
		if (m_waypoints.getIndex(start) == numWaypoints) {
			LogError("Link " << link->getUID() << " has an incorrect waypoint " << start);
			valid = false;
		}
		if (m_waypoints.getIndex(end) == numWaypoints) {
			LogError("Link " << link->getUID() << " has an incorrect waypoint " << end);
			valid = false;
		}
	}

	// Cells
	for (const auto& cell : m_cells.getIndexedData()) {
		const auto& vertices = cell->getVertices();
		for (VertexScheme::UidType vertex : vertices) {
			if (m_vertices.get(vertex) == nullptr) {
				LogError("Cell " << cell->getUID() << " has an incorrect vertex " << vertex);
				valid = false;
			}
		}

		// Make sure that the cell is cyclic
		if (vertices.front() != vertices.back()) {
			LogError("Cell " << cell->getUID() << " is not a closed polyChain, make sure the last vertex is the same as the first.");
			valid = false;
		}
	}

	// Objects
	for (const auto& object : m_objects.getIndexedData()) {
		const auto& vertices = object->getVertices();
		for (VertexScheme::UidType vertex : vertices) {
			if (m_vertices.get(vertex) == nullptr) {
				LogError("Object " << object->getUID() << " has an incorrect vertex " << vertex);
				valid = false;
			}
		}
	}

	// Zones
	for (const auto& zone : m_zones.getIndexedData()) {
		const auto& vertices = zone->getVertices();
		for (VertexScheme::UidType vertex : vertices) {
			if (m_vertices.get(vertex) == nullptr) {
				LogError("Zone " << zone->getUID() << " has an incorrect vertex " << vertex);
				valid = false;
			}
		}

		// Check that the zone is cylcic
		const auto& startVertex = m_vertices.get(vertices[0]);
		const auto& endVertex = m_vertices.get(vertices[vertices.size() - 1]);
		if (startVertex->getUID() != endVertex->getUID()) {
			LogError("Zone " << zone->getUID() << " is not closed. First and last vertex are not identical. First index: " << startVertex->getUID() << ", last index: " << endVertex->getUID());
		}
	}

	// Weatherstation
	if (m_weatherstations.size() == 0) {
		LogWarning("No weather station present in the Area database.");
	}
	for (const auto& weatherstation : m_weatherstations.getIndexedData()) {
		const VertexScheme::UidType vertexUID = weatherstation->getVertexUID();
		if (m_vertices.get(vertexUID) == nullptr) {
			LogError("Weather Station " << weatherstation->getUID() << " has an incorrect vertex " << vertexUID);
			valid = false;
		}

		// Check that the wind table exists
		if (!m_windStrengthTables.contains(weatherstation->getWindTableUid())) {
			LogError("Unknow wind strength table " << weatherstation->getWindTableUid() << " found in weather station " << weatherstation->getUID());
		}

		// Check if the current table exists
		if (!m_currentTables.contains(weatherstation->getCurrentTableUid())) {
			LogError("Curent table " << weatherstation->getWindTableUid() << " not found in weather station " << weatherstation->getUID());
		}
	}

	// Ertv
	for (const auto& weatherstation : m_weatherstations.getIndexedData()) {
		const VertexScheme::UidType vertexUID = weatherstation->getVertexUID();
		if (m_vertices.get(vertexUID) == nullptr) {
			LogError("Waypoint " << weatherstation->getUID() << " has an incorrect vertex " << vertexUID);
			valid = false;
		}
	}


	// Windstrength
	for (const auto& table : m_windStrengthTables) {
		int scale = 0;
		double totalProbability = 0;
		for (const auto& data : table.second) {
			if (scale > 12) {
				LogError("To many scales in wind strength table " << table.first);
				valid = false;
			}
			if (data->getScale() != scale) {
				LogError("Missing scale " << scale << " in table " << table.first);
				valid = false;
			}
			if (!m_windDirectionTables.contains(data->getDirectionTableUid())) {
				LogError("Can't find wave direction table " << data->getDirectionTableUid() << " specified in wind strength entry " << data->getUID());
				valid = false;
			}
			totalProbability += data->getProbability();
			scale++;
		}
		if (std::abs(1.0 - totalProbability) > 1e-6) {
			LogError("Probability of wind strength table " << table.first << " does not add up to one, but to " << totalProbability);
			valid = false;
		}
	}


	// Winddirection
	for (const auto& table : m_windDirectionTables) {
		std::set<double> angles;
		size_t numAngles = 0;
		for (const auto& data : table.second) {
			double angle = data->getAngle();
			numAngles++;
			if (angle < 0 || angle >= 2.0 * std::numbers::pi) {
				LogError("wind direction table " << table.first << " has an incorrect angle of " << angle << ". Expecting a value in the range of [0, 2 pi).");
			}
			angles.insert(angle);
		}

		// Check that all angles are unique
		if (numAngles != angles.size()) {
			LogError("Not all angle entries in wind direction table " << table.first << " are unique.");
		}
	}

	// Current
	for (const auto& currentTable : m_currentTables)
	{
		double sum = 0;
		for (const auto& entry : currentTable.second) {
			if (entry->getVelocity() < 0) {
				LogError("The magnitude of the current velocity in table " << currentTable.first << " is negative.");
				valid = false;
			}
			sum += entry->getProbability();
		}
		if (std::abs(sum - 1) > 1e-6) {
			LogError("Probability summation of current table " << currentTable.first << " does not sum up to one.");
			valid = false;
		}
	}

	return valid;
}

} // namespace tdb  
