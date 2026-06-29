// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include "util/IModel.h"

//#include <tdb/Waypoint.h>
//#include <tdb/Link.h>
//#include <tdb/TrafficNetwork.h>
//#include <tdb/ShipCategory.h>
//#include <tdb/HarmonicSpeed.h>
//#include <tdb/ShipLink.h>
#include <tdb/Area.h>

#include <tdb/Model.h>
#include <fstream>


namespace tdb {

//bool DataTables::readAllTables(std::string_view path)
//{
//	BaseModel baseModel;
//	//baseModel.readFile(path, "Waypoints.csv", waypoints);
//	//baseModel.readFile(path, "Links.csv", links);
//	//baseModel.readFile(path, "TrafficNetworks.csv", trafficNetworks);
//	//baseModel.readFile(path, "ShipCategories.csv", shipCategories);
//	//baseModel.readFile(path, "HarmonicSpeeds.csv", harmonicSpeeds);
//	//baseModel.readFile(path, "Ship_Links.csv", shipLinks);
//	//baseModel.readFile(path, "TrafficDBs.csv", trafficDBs);
//	//baseModel.readFile(path, "Areas.csv", areas);
//	return true; // TODO this makes no sense at the moment
//}
//
//bool DataTables::readAllTables(std::string_view path, std::unordered_map<std::string, std::string>&& tables)
//{
//	BaseModel baseModel;
//	//baseModel.readFile(path, tables["waypoints"], waypoints);
//	//baseModel.readFile(path, tables["links"], links);
//	//baseModel.readFile(path, tables["trafficNetworks"], trafficNetworks);
//	//baseModel.readFile(path, tables["shipCategories"], shipCategories);
//	//baseModel.readFile(path, tables["harmonicSpeeds"], harmonicSpeeds);
//	//baseModel.readFile(path, tables["shipsOnLinks"], shipLinks);
//	//baseModel.readFile(path, tables["trafficDBs"], trafficDBs);
//	//baseModel.readFile(path, tables["areas"], areas);
//	return true; // TODO this makes no sense at the moment. the readFile always returns true, need to do something here
//	// TODO make model specific tables in the model...
//}

} // namespace tdb



