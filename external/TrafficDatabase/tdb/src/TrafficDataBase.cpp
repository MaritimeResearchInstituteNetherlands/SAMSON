// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/TrafficDataBase.h>

namespace tdb {

TrafficDataBase::TrafficDataBase()
	: m_areaManager(m_areaModel)
{
}



void TrafficDataBase::setExposureModel(std::unique_ptr<tdb::BaseModel>& model) {
	m_exposureModel = std::move(model);
}



void TrafficDataBase::load(const std::string& inputFolder, FileLocations& fileLocations)
{
	m_areaModel.loadModel(inputFolder, fileLocations["areaModel"]);
	m_trafficModel.loadModel(inputFolder, fileLocations["trafficModel"]);
	m_exposureModel->loadModel(inputFolder, fileLocations["interactionModel"]); // TODO think of a better name of exposure model accidentModel?
}

} // namespace tdb
