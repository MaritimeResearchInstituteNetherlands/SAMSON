// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sim_export.h"
#include <tdb/AreaModel.h>
#include <tdb/TrafficModel.h>

#include <sml/Coordinate.h>

#include <sio/csv/csv_iarchive.h>
#include <sio/csv/csv_oarchive.h>

#include <vector>
#include <algorithm>

namespace sim
{

class ModelEntity
{
public:
	ModelEntity(const size_t& index) : m_index(index) {}

	inline void addCoordinate(const sml::Coordinate2D& coordinate) { m_coordinates.push_back(coordinate); }

	inline const size_t& getIndex() const { return m_index; }

	inline const std::vector<sml::Coordinate2D> getCoordinates() const { return m_coordinates; }

private:
	const size_t m_index;							// Index of the model entity in the database
	std::vector<sml::Coordinate2D> m_coordinates;	// Coordinates of the geometry
};



class TrafficEntity
{
public:
	TrafficEntity(const tdb::TrafficModel& trafficModel)	
		: m_trafficModel(trafficModel)
		, m_trafficIndex()
		, m_dataIndices()
		, m_shipCategoryDataIndices()
		, m_numShips(0)
	{
	}

	inline void addShip(const size_t& trafficIndex, const size_t& dataIndex, const size_t& shipCatIndex)
	{ 
		m_trafficIndex.push_back(trafficIndex);
		m_dataIndices.push_back(dataIndex);
		m_shipCategoryDataIndices.push_back(shipCatIndex);
		m_numShips++;
	}

	inline const size_t getNumShips() const { return m_numShips; }

	inline const size_t getShipCategoryIndex(size_t i) const { return m_shipCategoryDataIndices[i]; }

protected:
	std::vector<size_t> m_trafficIndex;
	std::vector<size_t> m_dataIndices;
	std::vector<size_t> m_shipCategoryDataIndices;
	size_t m_numShips;
	const tdb::TrafficModel& m_trafficModel;					// Reference to the ship category data
};



class IInteractionEntity
{
public:
	IInteractionEntity() {}

	virtual void setup() = 0;

	virtual void computeExposures() = 0;

	virtual void computeEvents() = 0;

	virtual void computeCollisions() = 0;
};

} // namespace sim

