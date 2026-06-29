// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/CausationModel.h>

namespace sem
{
bool CausationModel::loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames)
{
	loadTable(path, filenames, "causationFactors", m_causationFactorsDataBase);
	indexTable(m_causationFactorsDataBase, m_causationFactors);

	return validate();
}



void CausationModel::help()
{
	std::string header;
	std::ofstream dummy;
	boost::archive::csv_oarchive oa(dummy);

	LogInfo("SEM CausationModel contains the following tables:");
	header = oa.generateHeader(CausationFactorsScheme());
	LogInfo("\t causationFactors:");
	LogInfo("\t\t " << header);
}



bool CausationModel::validate()
{
	// Do we want to verify here that all ship classes exist?
	return true;
}
} // namespace sem

