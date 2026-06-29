// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sim/IExposureModel.h>
#include <tdb/Model.h>

#include "sem_export.h"
#include <sem/DataModel.h>
#include <sem/CausationModel.h>
#include <sem/MeasureModel.h>
#include <sem/DriftModelGenerator.h>
#include "yaml-cpp/yaml.h"

#include <algorithm>

namespace sem
{

class SEM_EXPORT SamsonExposureModel : public sim::IExposureModel
{
public:
	SamsonExposureModel();

	bool loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames) final;

	void help() final;

	void setup(const tdb::AreaModel& areaModel, const tdb::TrafficModel& trafficModel) final;

	const size_t getShipLinkDataIndex(const sim::ShipLinkData::UidType& uid) const final;

	const size_t getShipCellDataIndex(const sim::ShipCellData::UidType& uid) const final;

	std::unique_ptr<sim::Link> createLink(const size_t& linkIndex, const tdb::Data<tdb::LinkScheme>& linkScheme, const tdb::TrafficDataBase& trafficDataBase) const final;
	
	std::unique_ptr<sim::Cell> createCell(const size_t& cellIndex, const tdb::Data<tdb::CellScheme>& linkScheme, const tdb::TrafficDataBase& trafficDataBase) const final;
	
	std::unique_ptr<sim::Object> createObject(const size_t& objectIndex, const tdb::Data<tdb::ObjectScheme>& linkScheme, const std::vector<sml::Coordinate2D>& coordinates) const final;

	std::unique_ptr<sim::LinkLink> createLinkLink(const sim::Link& linkA, const sim::Link& linkB, const tdb::TrafficDataBase& trafficDataBase) const final;

	std::unique_ptr<sim::LinkCell> createLinkCell(const sim::Link& link, const sim::Cell& cell, const tdb::TrafficDataBase& trafficDatabase) const final;

	std::unique_ptr<sim::LinkObject> createLinkObject(const sim::Link& link, const tdb::TrafficDataBase& trafficDataBase, const tdb::WeatherstationScheme& wss) const final;

	std::unique_ptr<sim::CellObject> createCellObject(const sim::Cell& cell, const tdb::TrafficDataBase& trafficDataBase, const tdb::WeatherstationScheme& wss) const final;

	bool isLinkLinkPair(const sim::Link& linkA, const sim::Link& linkB) const final;

	bool isLinkCellPair(const sim::Link& linkIndex, const sim::Cell& cellIndex) const final;

	bool isLinkObjectPair(const size_t& linkIndex, const size_t& objectIndex) const final;

	bool isCellObjectPair(const size_t& cellIndex, const size_t& objectIndex) const final;

	inline const DataModel& getDataModel() const { return m_dataModel; }

	inline const CausationModel& getCausationModel() const { return m_causationModel; }

	inline const MeasureModel& getMeasureModel() const { return m_measureModel; }

	const std::vector<DriftModel>& getDriftModels(const tdb::WeatherstationScheme& weatherStationScheme) const;

	const std::vector<double>& getDriftDiscretisation(const tdb::WeatherstationScheme& weatherStationScheme) const;

	inline bool getUseAvoidance() const { return m_useAvoidance; }

	inline bool getUsePassingDistance() const { return m_usePassingDistance; }

private:
	void readMeasureConfig();

	// Input database
	DataModel m_dataModel; // TODO perhaps rename this to exposure model and this class to Samson Standard Model
	CausationModel m_causationModel;
	MeasureModel m_measureModel;

	// Configuration of the measures
	YAML::Node m_measureConfig;
	bool m_useEngineRepair;
	bool m_useAnchoring;
	bool m_usePassingDistance;
	bool m_useAvoidance;

	// Processed enviroment data for drift computations
	std::vector<std::vector<DriftModel>> m_driftModels;
	std::map<std::string,std::vector<double>> m_driftDiscretisation;
};

// TODO find a good place for these generic functions which are used in multiple interaction models
double getShipSpeed(const double u, const tdb::ShipCategoryScheme& shipCatData);

struct CollisionProjection {
	double length;
	double struckChance_i;
	double struckChance_j;
};

CollisionProjection getCollisionProjection(const double cosAngle, const double sinAngle, const double u_i, const double u_j, const double u_ij,	const tdb::ShipCategoryScheme& shipCatData_i, const tdb::ShipCategoryScheme& shipCatData_j);
} // namespace sem

