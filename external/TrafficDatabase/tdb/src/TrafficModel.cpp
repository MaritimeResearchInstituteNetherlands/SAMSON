// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/TrafficModel.h>

namespace tdb 
{

TrafficModel::TrafficModel()
{
}



bool TrafficModel::loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames)
{
	// Load model files
	loadTable(path, filenames, "shipCategories", m_trafficModelDataBase.shipCategoryDataBase);
	loadTable(path, filenames, "shipsOnLinks", m_trafficModelDataBase.shipLinkDataBase);
	loadTable(path, filenames, "shipsOnCells", m_trafficModelDataBase.shipCellDataBase);

	// Setup indexed tables for fast reference
	indexTable(m_trafficModelDataBase.shipCategoryDataBase, m_shipCategories);
	indexTable(m_trafficModelDataBase.shipLinkDataBase, m_shipLinks);
	indexTable(m_trafficModelDataBase.shipCellDataBase, m_shipCells);

	return validate();
}



void TrafficModel::help()
{
	std::string header;
	std::ofstream dummy;
	boost::archive::csv_oarchive oa(dummy);

	LogInfo("TrafficModel contains the following tables:");
	header = oa.generateHeader(ShipCategoryScheme());
	LogInfo("\t shipCategories:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(ShipLinkScheme());
	LogInfo("\t shipsOnLinks:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(ShipCellScheme());
	LogInfo("\t shipsOnCells:");
	LogInfo("\t\t " << header);
}



bool TrafficModel::validate()
{
	for (const auto& entry : m_trafficModelDataBase.shipCategoryDataBase) {
		if (entry->getDraft() <= 0) {
			LogFatal("Expecting a draft larger than zero in shipcategory " << entry->getUID());
		}
		if (entry->getLength() <= 0) {
			LogWarning("Expecting a length larger than zero in shipcategory " << entry->getUID());
		}
		if (entry->getHeight() <= 0) {
			LogWarning("Expecting a height larger than zero in shipcategory " << entry->getUID());
		}
		if ((entry->getHeight() - entry->getDraft()) <= 0.0) {
			LogWarning("shipcategory " << entry->getUID() << " is submerged.");
		}
	}
	return true;
}

} // namespace tdb
