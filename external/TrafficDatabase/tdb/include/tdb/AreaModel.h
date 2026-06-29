// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once
#include "tdb_export.h"
#include <tdb/Model.h>
#include <tdb/AreaSchemes.h>

namespace tdb {

// Database components that define purely geometric objects through vertices
// TODO this second template parameter only works with string, cant use size_t 
using IndexedVertices = IndexedDataTable<VertexScheme, std::string>;
using IndexedWaypoints = IndexedDataTable<WaypointScheme, WaypointScheme::UidType>;
using IndexedLinks = IndexedDataTable<LinkScheme, LinkScheme::UidType>;
using IndexedCells = IndexedDataTable<CellScheme, CellScheme::UidType>;
using IndexedObjects = IndexedDataTable<ObjectScheme, ObjectScheme::UidType>;
using IndexedZones = IndexedDataTable<ZoneScheme, ZoneScheme::UidType>;
using IndexedWeatherstations = IndexedDataTable<WeatherstationScheme, WeatherstationScheme::UidType>;
using IndexedErtvs = IndexedDataTable<ErtvScheme, ErtvScheme::UidType>;
using WindstrengthTable = DataTable<WindstrengthScheme >;
using Winddirection = DataTable<WinddirectionScheme>;
using CurrentTable = DataTable<CurrentScheme>;
using TideTable = DataTable<TideScheme>;
using IndexedWindstengthTables = std::unordered_map<std::string, WindstrengthTable>;
using IndexedWinddirectionTables = std::unordered_map<std::string, Winddirection>;
using IndexedCurrentTables = std::unordered_map<std::string, CurrentTable>;
using IndexedTideTables = std::unordered_map<std::string, TideTable>;



struct AreaDataBase
{
	DataTable<VertexScheme> vertexDataBase;
	DataTable<WaypointScheme> waypointDataBase;
	DataTable<LinkScheme> linkDataBase;
	DataTable<CellScheme> cellDataBase;
	DataTable<ObjectScheme> objectDataBase;
	DataTable<ZoneScheme> zoneDataBase;
	DataTable<WeatherstationScheme> weatherstationDataBase;
	DataTable<ErtvScheme> ertvDataBase;
	DataTable<WindstrengthScheme> windstrengthDataBase;
	DataTable<WinddirectionScheme> winddirectionDataBase;
	DataTable<CurrentScheme> currentDataBase;
	DataTable<TideScheme> tideDataBase;
};



class TDB_EXPORT AreaModel : public BaseModel
{
public:
	AreaModel();

	bool loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames) final;

	void help() final;

	bool validate();

	void indexDataBase();

	inline AreaDataBase& getAreaDataBase() { return m_areaDataBase; }

	inline const AreaDataBase& getAreaDataBase() const { return m_areaDataBase; }

	inline const IndexedVertices& getVertices() const { return m_vertices; }

	inline const IndexedWaypoints& getWayPoints() const { return m_waypoints; }

	inline const IndexedLinks& getLinks() const { return m_links; }

	inline const IndexedCells& getCells() const { return m_cells; }

	inline const IndexedObjects& getObjects() const { return m_objects; }

	inline const IndexedZones& getZones() const { return m_zones; }

	inline const IndexedWeatherstations& getWeatherstations() const { return m_weatherstations; }

	inline const IndexedErtvs& getErtvs() const { return m_ertvs; }

	inline const IndexedWindstengthTables& getWindstrengthTables() const { return m_windStrengthTables; }

	inline const IndexedWinddirectionTables& getWinddirectionTables() const { return m_windDirectionTables; }

	inline const IndexedCurrentTables& getCurrentTables() const { return m_currentTables; }

	inline const IndexedTideTables& getTideTables() const { return m_tideTables; }

private:
	AreaDataBase m_areaDataBase;

	// Indexed data tables
	IndexedVertices m_vertices;
	IndexedWaypoints m_waypoints;
	IndexedLinks m_links;
	IndexedCells m_cells;
	IndexedObjects m_objects;
	IndexedZones m_zones;
	IndexedWeatherstations m_weatherstations;
	IndexedErtvs m_ertvs;
	IndexedWindstengthTables m_windStrengthTables;
	IndexedWinddirectionTables m_windDirectionTables;
	IndexedCurrentTables m_currentTables;
	IndexedTideTables m_tideTables;
};
} // namespace tdb
