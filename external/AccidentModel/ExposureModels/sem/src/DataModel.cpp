// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sem/DataModel.h>

namespace sem
{
bool DataModel::loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames)
{
	// TODO add header checks
	loadTable(path, filenames, "shipLinkData", m_shipLinkDataBase);
	loadTable(path, filenames, "shipCellData", m_shipCellDataBase);

	// Setup indexed table
	indexTable(m_shipLinkDataBase, m_shipLinkData);
	indexTable(m_shipCellDataBase, m_shipCellData);


	return validate();
}



bool DataModel::validate()
{
	return true;
}



void DataModel::help()
{
	std::string header;
	std::ofstream dummy;
	boost::archive::csv_oarchive oa(dummy);

	LogInfo("SEM DataModel contains the following tables:");
	header = oa.generateHeader(ShipLinkDataScheme());
	LogInfo("\t shipLinkData:");
	LogInfo("\t\t " << header);
	header = oa.generateHeader(ShipCellDataScheme());
	LogInfo("\t shipCelldata:");
	LogInfo("\t\t " << header);
}



GeneralModelData::GeneralModelData()
{
	// Hard coded values based on wind force
	m_windSpeeds.resize(13);
	m_windSpeeds[0] = 0.001;
	m_windSpeeds[1] = 1.003;
	m_windSpeeds[2] = 2.57;
	m_windSpeeds[3] = 4.37;
	m_windSpeeds[4] = 6.95;
	m_windSpeeds[5] = 9.77;
	m_windSpeeds[6] = 12.59;
	m_windSpeeds[7] = 15.68;
	m_windSpeeds[8] = 19.02;
	m_windSpeeds[9] = 22.62;
	m_windSpeeds[10] = 26.47;
	m_windSpeeds[11] = 30.58;
	m_windSpeeds[12] = 0.01;

	m_peakPeriod.resize(13);
	m_peakPeriod[0] = 0.001;
	m_peakPeriod[1] = 0.78;
	m_peakPeriod[2] = 1.87;
	m_peakPeriod[3] = 3.06;
	m_peakPeriod[4] = 4.62;
	m_peakPeriod[5] = 6.21;
	m_peakPeriod[6] = 7.7;
	m_peakPeriod[7] = 9.24;
	m_peakPeriod[8] = 10.81;
	m_peakPeriod[9] = 12.44;
	m_peakPeriod[10] = 14.09;
	m_peakPeriod[11] = 15.79;
	m_peakPeriod[12] = 0.01;

	m_pAnchorFailure.resize(13);
	m_pAnchorFailure[0] = 0.01;
	m_pAnchorFailure[1] = 0.01;
	m_pAnchorFailure[2] = 0.01;
	m_pAnchorFailure[3] = 0.01;
	m_pAnchorFailure[4] = 0.035;
	m_pAnchorFailure[5] = 0.07;
	m_pAnchorFailure[6] = 0.126;
	m_pAnchorFailure[7] = 0.21;
	m_pAnchorFailure[8] = 0.35;
	m_pAnchorFailure[9] = 0.49;
	m_pAnchorFailure[10] = 0.63;
	m_pAnchorFailure[11] = 0.7;
	m_pAnchorFailure[12] = 1.0;


	m_localRamBounds = sml::AngleBounds(sml::normalisedPositiveAngles(-3.0 * GeneralModelData::ramSigma()),
		sml::normalisedPositiveAngles(3.0 * GeneralModelData::ramSigma()),
		6.0 * GeneralModelData::ramSigma());

	LogInfo("Weather data based on wind force: ");
	for (size_t i = 0; i < 13; i++) {
		LogInfo("- Scale: " << i << ", wind speed: " << m_windSpeeds[i] << ", peak period: " << m_peakPeriod[i] << ", anchor failure probability: " << m_pAnchorFailure[i]);
	}

	LogInfo("Constant model parameters: ");
	LogInfo("- Gravitational acecleration: " << g());
	LogInfo("- Water density: " << rhoWater());
	LogInfo("- Air density: " << rhoAir());
	LogInfo("- Hull drag coefficient: " << hullDragCoefficient());
	LogInfo("- wind drag coefficient: " << windDragCoefficient());
	LogInfo("- Maximum size of a sublink bin in LinkObject interactions: " << maxSubLinkBinSize());
}
} // namespace sem


