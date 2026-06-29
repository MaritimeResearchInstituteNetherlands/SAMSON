// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/ExposureModel.h>
#include <sem/ModelTypes.h>
#include <sml/GeoUtils.h>
#include <sml/Polygon.h>
#include <tdb/TrafficDataBase.h>

namespace sem
{
SamsonExposureModel::SamsonExposureModel()
	: m_useEngineRepair(true)
	, m_useAnchoring(true)
	, m_usePassingDistance(false)
	, m_useAvoidance(true)
{
}



bool SamsonExposureModel::loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames)
{
	// Set up general weather data based on wind force
	LogInfo("Creating samson exposure model");
	GeneralModelData::get();

	// Load submodels
	bool valid = true;
	if (!m_dataModel.loadModel(path, filenames)) valid = false;
	if (!m_causationModel.loadModel(path, filenames)) valid = false;
	if (!m_measureModel.loadModel(path, filenames)) valid = false;

	// Load config

	if (!filenames.contains("measuresConfig")) {
		LogWarning("Filename for measuresConfig not found.")
		valid = false;
	}
	else {
		std::string filename = std::string(path) + std::string(filenames["measuresConfig"][0]);
		try {
			m_measureConfig = YAML::LoadFile(filename);
		}
		catch (...) {
			LogError("An error occured while tring to read measureConfig. Filename may be incorrect: " << filename)
		}
		auto apiNode = m_measureConfig["apiVersion"];
		if (!apiNode) {
			LogWarning("The property apiVersion of " << filename << " could not be found.");
		}
		std::string inputApiVersion = apiNode.as<std::string>();
		if (!((inputApiVersion.compare("0.0.1")) == 0)) {
			LogError("Expecting an apiVersion of 0.0.1");
		}
	}

	return valid;
}



void SamsonExposureModel::help()
{
	DataModel dataModel;
	CausationModel causationModel;
	MeasureModel measureModel;

	dataModel.help();
	causationModel.help();
	measureModel.help();
}



void SamsonExposureModel::setup(const tdb::AreaModel& areaModel, const tdb::TrafficModel& trafficModel)
{
	readMeasureConfig();

	LogInfo("Computing drift models.");
	size_t numWeatherstations = areaModel.getWeatherstations().size();
	size_t numShipCategories = trafficModel.getShipCategories().size();

	m_driftModels.resize(numWeatherstations);
	for (size_t i = 0; i < numWeatherstations; i++) {
		// Set up a generic drift model for this weather station
		const auto& ws = areaModel.getWeatherstations().get(i);
		DriftModelGenerator driftModelGenerator(areaModel, *ws, m_useEngineRepair, m_useAnchoring);

		// Compute the ship-specific drift model
		m_driftModels[i].resize(numShipCategories);
		for (size_t j = 0; j < numShipCategories; j++) {
			const auto& sc = trafficModel.getShipCategories().get(j);
			driftModelGenerator.compute(m_driftModels[i][j], sc->getUID(), sc->getDraft(), sc->getHeight());
		}

		// Create drift angle discretisation
		DriftModel driftAngleModel;
		driftModelGenerator.compute(driftAngleModel, "", 0.01, 0.01);
		m_driftDiscretisation[ws->getUID()] = driftAngleModel.getDiscretisation();
	}
}



const size_t SamsonExposureModel::getShipLinkDataIndex(const sim::ShipLinkData::UidType& uid) const
{
	return m_dataModel.getIndexedShipLinkData().getIndex(uid);
}



const size_t SamsonExposureModel::getShipCellDataIndex(const sim::ShipCellData::UidType& uid) const
{
	return m_dataModel.getIndexedShipCellData().getIndex(uid);
}



std::unique_ptr<sim::Link> SamsonExposureModel::createLink(const size_t& linkIndex, const tdb::Data<tdb::LinkScheme>& linkScheme, const tdb::TrafficDataBase& trafficDataBase) const
{
	return std::make_unique<sem::Link>(linkIndex, linkScheme, *this, trafficDataBase);
}
	


std::unique_ptr<sim::Cell> SamsonExposureModel::createCell(const size_t& cellIndex, const tdb::Data<tdb::CellScheme>& cellScheme, const tdb::TrafficDataBase& trafficDataBase) const
{
	return std::make_unique<sem::Cell>(cellIndex, cellScheme, *this, trafficDataBase);
}
	


std::unique_ptr<sim::Object> SamsonExposureModel::createObject(const size_t& objectIndex, const tdb::Data<tdb::ObjectScheme>& objectScheme, const std::vector<sml::Coordinate2D>& coordinates) const
{
	return std::make_unique<sem::Object>(objectIndex, objectScheme, coordinates);
	
}



std::unique_ptr<sim::LinkLink> SamsonExposureModel::createLinkLink(const sim::Link& linkA, const sim::Link& linkB, const tdb::TrafficDataBase& trafficDataBase) const
{
	return std::make_unique<sem::LinkLink>(linkA, linkB, *this, trafficDataBase);
}



std::unique_ptr<sim::LinkCell> SamsonExposureModel::createLinkCell(const sim::Link& link, const sim::Cell& cell, const tdb::TrafficDataBase& trafficDatabase) const
{
	return std::make_unique<sem::LinkCell>(link, cell, *this, trafficDatabase);
}



std::unique_ptr<sim::LinkObject> SamsonExposureModel::createLinkObject(const sim::Link& link, const tdb::TrafficDataBase& trafficDataBase, const tdb::WeatherstationScheme& wss) const
{
	return std::make_unique<sem::LinkObject>(link, *this, trafficDataBase, wss);
}



std::unique_ptr<sim::CellObject> SamsonExposureModel::createCellObject(const sim::Cell& cell, const tdb::TrafficDataBase& trafficDataBase, const tdb::WeatherstationScheme& wss) const
{
	return std::make_unique<sem::CellObject>(cell, *this, trafficDataBase, wss);
}



// Conditions for a LinkLink are
// 1) When the endpoint of the links share the same way point, a LinkLink interaction is present (crossing,...)
// 2) When two opposite links share the same way points (head-on)
bool SamsonExposureModel::isLinkLinkPair(const sim::Link& linkA, const sim::Link& linkB) const
{
	const auto& wpAStart = linkA.getLinkScheme()->getStart();
	const auto& wpAEnd = linkA.getLinkScheme()->getEnd();
	const auto& wpBStart = linkB.getLinkScheme()->getStart();
	const auto& wpBEnd = linkB.getLinkScheme()->getEnd();
	if (wpAEnd == wpBEnd) {
		LogVerbose("Found crossing links " << linkA.getUID() << " and " << linkB.getUID());
		return true;
	}
		
	if (wpAEnd == wpBStart && wpBEnd == wpAStart) {
		LogVerbose("Found head-on links " << linkA.getUID() << " and " << linkB.getUID());
		return true;
	}

	return false;
}



bool SamsonExposureModel::isLinkCellPair(const sim::Link& link, const sim::Cell& cell) const
{
	// Create link polygon in the cell coordinates
	sml::Polygon p;
	const std::vector<sml::Coordinate2D> rectangle = link.getRectangle();
	for (size_t i = 0; i < rectangle.size() - 1; i++) { // Convert from polyline to polygon we dont need the last vertex
		p.addVertex(sml::GeoUtils::projectToPlane(cell.getOrigin(), rectangle[i]));
	}

	// Check if a vertex is inside the polygon
	std::vector<sml::Polygon> intersections = p.getIntersectionWith(cell.getPolygon());
	if (intersections.size() > 0) {
		return true;
	}

	return false;
}



bool SamsonExposureModel::isLinkObjectPair(const size_t& linkIndex, const size_t& objectIndex) const
{
	return true;
}



bool SamsonExposureModel::isCellObjectPair(const size_t& cellIndex, const size_t& objectIndex) const
{
	return true;
}



const std::vector<DriftModel>& SamsonExposureModel::getDriftModels(const tdb::WeatherstationScheme& weatherStationScheme) const
{
	for (const auto& entry : m_driftModels) {
		if (entry[0].getWeatherStationUID() == weatherStationScheme.getUID()) {
			return entry;
		}
	}

	LogError("Unexpected error has occurred while finding drift models for weather station " << weatherStationScheme.getUID());
	return m_driftModels[0];
}



const std::vector<double>& SamsonExposureModel::getDriftDiscretisation(const tdb::WeatherstationScheme& weatherStationScheme) const
{
	for (const auto& entry : m_driftDiscretisation) {
		if (entry.first == weatherStationScheme.getUID()) {
			return entry.second;
		}
	}
	LogError("Unexpected error has occurred while finding drift models for weather station " << weatherStationScheme.getUID());
	return m_driftDiscretisation.begin()->second;
}



void SamsonExposureModel::readMeasureConfig()
{
	LogInfo("Loading measures configuration");
	if (YAML::Node node = m_measureConfig["driftMeasures"]) {
		std::string engineRepair = "engineRepair";
		if (auto parameter = node[engineRepair]) {
			m_useEngineRepair = parameter.as<bool>();
		}
		std::string anchoring = "anchoring";
		if (auto parameter = node[anchoring]) {
			m_useAnchoring = parameter.as<bool>();
		}
		std::string passingDistance = "passingDistance";
		if (auto parameter = node[passingDistance]) {
			m_usePassingDistance = parameter.as<bool>();
		}
		LogInfo("\tDrift measures configuration:");
		LogInfo("\t\t" << engineRepair <<": " << std::boolalpha << m_useEngineRepair);
		LogInfo("\t\t" << anchoring <<": " << std::boolalpha << m_useAnchoring);
		LogInfo("\t\t" << passingDistance << ": " << std::boolalpha << m_usePassingDistance);
	}
	else {
		LogInfo("driftMeasures node in measuresConfig is not present.")
	}
	if (YAML::Node node = m_measureConfig["ramMeasures"]) {
		std::string avoidance = "avoidance";
		if (auto parameter = node[avoidance]) {
			m_useAvoidance = parameter.as<bool>();
		}
		LogInfo("\tRam measures configuration:");
		LogInfo("\t\t" << avoidance << ": " << std::boolalpha << m_useAvoidance);
	}
	else {
		LogInfo("ramMeasures node in measuresConfig is not present.")
	}
}



double getShipSpeed(double u, const tdb::ShipCategoryScheme& shipCatData)
{
	if (std::isnan(u)) {
		u = shipCatData.getDefaultSpeed();
	}
	return u;
}



CollisionProjection getCollisionProjection(const double cosAngle, const double sinAngle, const double u_i, const double u_j, const double u_ij, const tdb::ShipCategoryScheme& shipCatData_i, const tdb::ShipCategoryScheme& shipCatData_j)
{
	// Grab the values
	const double l_i = shipCatData_i.getLength();
	const double l_j = shipCatData_j.getLength();
	const double b_i = shipCatData_i.getWidth();
	const double b_j = shipCatData_j.getWidth();

	// Compute the indicator!
	double d_j = b_i;
	double d_i = b_j;
	if (sinAngle != 0.0) {
		const double val_j = sinAngle * u_j / u_ij;
		const double val_i = sinAngle * u_i / u_ij;
		d_j = l_i * u_j * sinAngle / u_ij + b_i * std::sqrt(1.0 - val_j * val_j);
		d_i = l_j * u_i * sinAngle / u_ij + b_j * std::sqrt(1.0 - val_i * val_i);
	}

	const double d_ij = d_i + d_j;
	double d_ij_inv = 0.0;
	if (d_ij != 0.0) {
		d_ij_inv = 1.0 / d_ij;
	}

	CollisionProjection projection;
	projection.length = d_ij;
	projection.struckChance_i = d_j * d_ij_inv;
	projection.struckChance_j = d_i * d_ij_inv;

	return projection;
}

} // namespace sem

