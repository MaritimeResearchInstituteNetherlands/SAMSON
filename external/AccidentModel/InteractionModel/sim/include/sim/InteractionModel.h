// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sim_export.h"
#include <sim/InteractionTypes.h>
#include <sim/IExposureModel.h>
#include <tdb/TrafficDataBase.h>
#include <sml/AngularDistributionFunction.h>

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <execution>
#include <thread>

namespace sim
{
class SIM_EXPORT InteractionModel
{
public:
	InteractionModel(tdb::TrafficDataBase& trafficDataBase);

	InteractionModel(const InteractionModel&) = delete;

	InteractionModel& operator=(const InteractionModel&) = delete;
	
	void setup();

	void reset();

	void computeExposures();

	void computeEvents();

	void computeCollisions();

	inline const std::vector<std::unique_ptr<Link>>& getLinks() const { return m_links; }

	inline const std::vector<std::unique_ptr<Cell>>& getCells() const {	return m_cells; }

	inline const std::vector<std::unique_ptr<Object>>& getObjects() const { return m_objects; }

	inline const std::vector<std::unique_ptr<LinkLink>>& getLinkLinks() const { return m_linkLinks; }

	inline const std::vector<std::unique_ptr<LinkCell>>& getLinkCells() const { return m_linkCells; }

	inline const std::vector<std::unique_ptr<LinkObject>>& getLinkObjects() const { return m_linkObjects; }

	inline const std::vector<std::unique_ptr<CellObject>>& getCellObjects() const { return m_cellObjects; }

	inline const bool getExposuresDone() const { return m_exposuresDone; }

	inline const bool getEventsDone() const { return m_eventsDone; }

	inline const bool getCollisionsDone() const { return m_collisionsDone; }

	inline void setParallel(bool parallel) { m_parallel = parallel; }

private:
	// Create the indicator model link
	void createModelLinks();

	// Create the indicator model cell
	void createModelCells();

	// Create the indicator model object
	void createModelObjects();
	
	// Load the traffic on links
	void loadTrafficOnLinks();

	// Load the traffic on cells
	void loadTrafficOnCells();

	// With all data loaded, finalise the model enities
	void finaliseModelEntities();

	// Finalise the pair entities
	void finalisePairEntities();
	
	// Generate the pair interactions (LinkLink, LinkCell, LinkObject, CellObject)
	void createPairInteractions();

	template <class T, class F>
	void compute(std::vector<std::unique_ptr<T>>& list, std::string type, F function)
	{
		if (m_parallel) {

			const auto processor_count = std::max(size_t(std::thread::hardware_concurrency()), size_t(1));
			size_t groupSize = std::max(list.size() / processor_count, size_t(1));
			std::vector<std::pair<size_t, size_t>> groups;
			for (size_t i = 0; i < list.size(); i += groupSize) {
				groups.emplace_back(i, std::min(i + groupSize, list.size()));
			}

			std::for_each(std::execution::par_unseq, groups.begin(), groups.end(), [&](const auto& group) {
				for (size_t i = group.first; i < group.second; i++) {
					LogVerbose("Computing "<< type << " for " << list[i]->getDescription());
					function(list[i]);
				}
				});

			// This seems slower:
			//std::for_each(std::execution::par_unseq, list.begin(), list.end(), [&](const auto& item) {
			//	LogVerbose("Computing exposure for " << item->getDescription());
			//	item->computeExposures();
			//	});
		}
		else {
			std::for_each(std::execution::seq, list.begin(), list.end(), [&](std::unique_ptr<T>& item) {
				LogVerbose("Computing " << type << " for " << item->getDescription());
				function(item);
				});
		}
	}



	template <class T>
	void computeExposures(std::vector<std::unique_ptr<T>>& list, std::string type)
	{
		if (list.size() > 0) {
			LogInfo("Computing " << type << " exposures.");
		}

		compute(list, "exposures", [](std::unique_ptr<T>& item) { item->computeExposures();});
	}



	template <class T>
	void computeEvents(std::vector<std::unique_ptr<T>>& list, std::string type)
	{
		if (list.size() > 0) {
			LogInfo("Computing " << type << " events.");
		}

		compute(list, "events", [](std::unique_ptr<T>& item) { item->computeEvents();});
	}



	template <class T>
	void computeCollisions(std::vector<std::unique_ptr<T>>& list, std::string type)
	{
		if (list.size() > 0) {
			LogInfo("Computing " << type << " collisions.");
		}

		compute(list, "collisions", [](std::unique_ptr<T>& item) { item->computeCollisions();});
	}



	template <class T>
	void finaliseSetup(std::vector<std::unique_ptr<T>>& list)
	{
		for (auto item = list.begin(); item != list.end(); item++) {
			(*item)->setup();
		}
	}

	std::string findClosestWeatherStation(const sml::Coordinate2D& origin);

private:

	const tdb::TrafficDataBase& m_trafficDataBase;
	const tdb::AreaModel& m_areaModel;
	const tdb::TrafficModel& m_trafficModel;
	sim::IExposureModel& m_exposureModel; // TODO rename it to implementationModel or something, find a new suitable name

	// Model elements, traffic elements and single element interactions
	std::vector<std::unique_ptr<Link>> m_links;				// Can contain traffic
	std::vector<std::unique_ptr<Cell>> m_cells;				// Can contain traffic
	std::vector<std::unique_ptr<Object>> m_objects;			// Does not contain traffic and is not an interaction itself. Serves as building block for pair interactions
	
	// Pair interactions
	std::vector<std::unique_ptr<LinkLink>> m_linkLinks;
	std::vector<std::unique_ptr<LinkCell>> m_linkCells;
	std::vector<std::unique_ptr<LinkObject>> m_linkObjects;
	std::vector<std::unique_ptr<CellObject>> m_cellObjects;

	// Broadphase interaction detection grid
	// TODO: This is an optimisation technique and not directly required for the initial pipeline

	// Flow control
	bool m_parallel;
	bool m_exposuresDone;
	bool m_eventsDone;
	bool m_collisionsDone;
};





//class InteractionInstance
//{
//public:
//	InteractionInstance(const std::unique_ptr<InteractionModel>& baseModel)
//		: m_baseModel(baseModel)
//	{}
//
//
//
//private:
//	const std::unique_ptr<InteractionModel>& m_baseModel;
//
//	// List of links/cells/objects (linklink etc?) that are removed from the computation
//	// List of links/cells/objects (LinkLink etc?) that are added to the computation
//	// List of interactions that need to be recomputed
//};
} // namespace sim
