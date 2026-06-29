// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include<tdb_export.h>
#include <tdb/AreaSchemes.h>
#include <tdb/AreaModel.h>
#include <sml/Coordinate.h>

namespace tdb {
class TDB_EXPORT AreaManager
{
public:
	AreaManager(AreaModel& areaModel);

	bool command(std::stringstream& input);

	//struct GroupedUids {
	//	std::vector<std::string> waypointUids;
	//	std::vector<std::string> cell;
	//	std::vector<std::string> object;
	//	std::vector<std::string> weatherstation;
	//	std::vector<std::string> ertv;
	//};

	bool apply();

private:

	bool add(std::stringstream& input);

	bool modify(std::stringstream& input);

	bool remove(std::stringstream& input);

	bool addGrid(std::stringstream& input);

	bool addWaypoint(std::stringstream& input);

	bool addLink(std::stringstream& input);

	//bool addCell()

	//bool addObject()
	
	//bool addWeatherStation()

	//bool addErtv()

	bool modifyVertex(std::stringstream& input);

	bool removeGrid(const std::string& gridUID);

	//bool remove(const GroupedUids& groupedUids);
	//
	//void mapIndices(const AreaDataBase& areaDataBase, std::vector<int>& indexCount, const GroupedUids& groupedUids);

	void mapIndices(const AreaDataBase& areaDataBase, std::vector<int>& indexCount, const std::string& gridUID);

	void updateAreaIndices(const AreaDataBase& areaDataBase, std::vector<int>& indexMap);

	AreaModel& m_areaModel;

	std::vector<std::string> m_add;
	std::vector<std::string> m_modify;
	std::vector<std::string> m_remove;
};
} // namespace tdb
