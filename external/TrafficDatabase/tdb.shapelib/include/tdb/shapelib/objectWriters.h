// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma

#include <sio/shapelib/shapelibWriter.h>
#include <tdb/shapelib/baseObjectWriters.h>
#include <tdb/AreaModel.h>

namespace tdb {
namespace shapelib {

class WaypointObjectWriter : public BaseWaypointObjectWriter
{
	using Base = BaseWaypointObjectWriter;
public:
	WaypointObjectWriter(const IndexedWaypoints& waypoints, const IndexedVertices& vertices);
};



class LinkObjectWriter : public BaseLinkObjectWriter
{
	using Base = BaseLinkObjectWriter;
public:
	LinkObjectWriter(const IndexedLinks& links, const IndexedWaypoints& waypoints, const IndexedVertices& vertices);
};



class CellObjectWriter : public sio::shapelib::ObjectWriter
{
public:
	CellObjectWriter(const IndexedCells& cells, const IndexedVertices& vertices);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::polygon; }

	size_t size() const final { return m_cells.size(); }

private:
	std::vector<std::pair<double, double>> m_coordinates;
	const IndexedCells& m_cells;
	const IndexedVertices& m_vertices;
};



class ObjectObjectWriter : public BaseObjectObjectWriter
{
	using Base = BaseObjectObjectWriter;
public:
	ObjectObjectWriter(const IndexedObjects& objects, const IndexedVertices& vertices);
};



class ZoneObjectWriter : public sio::shapelib::ObjectWriter
{
public:
	ZoneObjectWriter(const IndexedZones& zones, const IndexedVertices& vertices);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::polygon; }

	size_t size() const final { return m_zones.size(); }

private:
	std::vector<std::pair<double, double>> m_coordinates;
	const IndexedZones& m_zones;
	const IndexedVertices& m_vertices;
};



class WeatherstationObjectWriter : public sio::shapelib::ObjectWriter
{
public:
	WeatherstationObjectWriter(const IndexedWeatherstations& weatherstations, const IndexedVertices& vertices);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::point; }

	size_t size() const final { return m_weatherstations.size(); }

private:
	std::vector<std::pair<double, double>> m_coordinates;
	const IndexedWeatherstations& m_weatherstations;
	const IndexedVertices& m_vertices;
};



class ErtvObjectWriter : public sio::shapelib::ObjectWriter
{
public:
	ErtvObjectWriter(const IndexedErtvs& ertvs, const IndexedVertices& vertices);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::point; }

	size_t size() const final { return m_ertvs.size(); }

private:
	std::vector<std::pair<double, double>> m_coordinates;
	const IndexedErtvs& m_ertvs;
	const IndexedVertices& m_vertices;
};

} //  namespace shapelib
} // namespace sim
