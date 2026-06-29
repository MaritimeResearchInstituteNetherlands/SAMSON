// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once
#include <tdb.shapelib_export.h>
#include <tdb/AreaModel.h>
#include <sio/shapelib/shapelibWriter.h>
#include <filesystem>

namespace tdb {
namespace shapelib {

class TDB_SHAPELIB_EXPORT BaseWaypointObjectWriter : public sio::shapelib::ObjectWriter
{
public:
	BaseWaypointObjectWriter(const IndexedWaypoints& waypoints, const IndexedVertices& vertices);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::point; }

	size_t size() const final { return m_waypoints.size(); }

protected:
	std::vector<std::pair<double, double>> m_coordinates;
	const IndexedWaypoints& m_waypoints;
	const IndexedVertices& m_vertices;
};



class TDB_SHAPELIB_EXPORT BaseLinkObjectWriter : public sio::shapelib::ObjectWriter
{
public:
	BaseLinkObjectWriter(const IndexedLinks& links, const IndexedWaypoints& waypoints, const IndexedVertices& vertices);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::arc; }

	size_t size() const final { return m_links.size(); }

protected:
	std::vector<std::pair<double, double>> m_coordinates;
	const IndexedLinks& m_links;
	const IndexedWaypoints& m_waypoints;
	const IndexedVertices& m_vertices;
};



class TDB_SHAPELIB_EXPORT BaseObjectObjectWriter : public sio::shapelib::ObjectWriter
{
public:
	BaseObjectObjectWriter(const IndexedObjects& objects, const IndexedVertices& vertices);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::arc; }

	size_t size() const final { return m_objects.size(); }

protected:
	std::vector<std::pair<double, double>> m_coordinates;
	const IndexedObjects& m_objects;
	const IndexedVertices& m_vertices;
};

} //  namespace shapelib
} // namespace tdb
