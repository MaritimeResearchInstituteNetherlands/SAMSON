// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <tdb_export.h>
#include <tdb/AreaModel.h>
#include <tdb/TrafficModel.h>
#include <tdb/AreaManager.h>

namespace tdb {

using FileLocations = std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>;

class TDB_EXPORT TrafficDataBase {
public:

	TrafficDataBase();

	void setExposureModel(std::unique_ptr<BaseModel>& model);

	void load(const std::string& inputFolder, FileLocations& fileLocations);

	inline const  AreaModel& getAreaModel() const { return m_areaModel; }

	inline const TrafficModel& getTrafficModel() const { return m_trafficModel; }

	inline std::unique_ptr<BaseModel>& getExposureModel() { return m_exposureModel; }

	inline AreaManager& getAreaManager() { return m_areaManager; }

private:
	AreaModel m_areaModel;
	TrafficModel m_trafficModel;
	std::unique_ptr<BaseModel> m_exposureModel;

	AreaManager m_areaManager;
	//TrafficManager m_trafficManager;
};
} //namespace tdb
