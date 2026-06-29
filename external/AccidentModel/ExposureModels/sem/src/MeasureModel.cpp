// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/MeasureModel.h>

namespace sem
{
bool MeasureModel::loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames)
{
	loadTable(path, filenames, "pilotassistanceShip", m_pilotAssistanceShipDataBase);
	loadTable(path, filenames, "pilotassistanceZones", m_pilotAssistanceZonesDataBase);

	indexTable(m_pilotAssistanceShipDataBase, m_pilotAssistanceShip);
	indexTable(m_pilotAssistanceZonesDataBase, m_pilotAssistanceZones);

	return validate();
}



void MeasureModel::help()
{
	std::string header;
	std::ofstream dummy;
	boost::archive::csv_oarchive oa(dummy);

	LogInfo("SEM MeasureModel contains the following tables:");
	header = oa.generateHeader(PilotAssistanceShipScheme());
	LogInfo("\t pilotassistanceShip:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(PilotAssistanceZoneScheme());
	LogInfo("\t pilotassistanceZone:");
	LogInfo("\t\t " << header);
}



bool MeasureModel::validate()
{
	// Do we want to verify here that all ship classes exist?
	return true;
}
} // namespace sem
