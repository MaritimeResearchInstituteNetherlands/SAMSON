// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma

#include <sio/shapelib/shapelibWriter.h>
#include <sim/InteractionModel.h>
#include <tdb/shapelib/baseObjectWriters.h>
#include <tdb/AreaModel.h>

namespace sim {
namespace shapelib {

class LinkPolygonWriter : public sio::shapelib::ObjectWriter
{
public:
	LinkPolygonWriter(const std::vector<std::unique_ptr<Link>>& links);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::polygon; }

	size_t size() const final { return m_links.size(); }

private:
	std::vector<std::pair<double, double>> m_coordinates;
	const std::vector<std::unique_ptr<Link>>& m_links;
};



class LinkRectangleWriter : public sio::shapelib::ObjectWriter
{
public:
	LinkRectangleWriter(const std::vector<std::unique_ptr<Link>>& links);

	const std::vector<std::pair<double, double>>& getCoordinates(size_t index) final;

	sio::shapelib::shapeType getType() const final { return sio::shapelib::shapeType::polygon; }

	size_t size() const final { return m_links.size(); }

private:
	std::vector<std::pair<double, double>> m_coordinates;
	const std::vector<std::unique_ptr<Link>>& m_links;
};



class OvertakingResultsWriter : public tdb::shapelib::BaseLinkObjectWriter
{
	using Base = tdb::shapelib::BaseLinkObjectWriter;
public:
	OvertakingResultsWriter(const InteractionModel& model, const tdb::IndexedLinks& links, const tdb::IndexedWaypoints& waypoints, const tdb::IndexedVertices& vertices);
};



class CrossingResultsWriter : public tdb::shapelib::BaseWaypointObjectWriter
{
	using Base = tdb::shapelib::BaseWaypointObjectWriter;
public:
	CrossingResultsWriter(const sim::InteractionModel& model, const tdb::IndexedWaypoints& waypoints, const tdb::IndexedVertices& vertices);
};



class HeadonResultsWriter : public tdb::shapelib::BaseLinkObjectWriter
{
	using Base = tdb::shapelib::BaseLinkObjectWriter;
public:
	HeadonResultsWriter(const InteractionModel& model, const tdb::IndexedLinks& links, const tdb::IndexedWaypoints& waypoints, const tdb::IndexedVertices& vertices);
};



class DriftResultWriter : public tdb::shapelib::BaseObjectObjectWriter
{
	using Base = tdb::shapelib::BaseObjectObjectWriter;
public:
	DriftResultWriter(const sim::InteractionModel& model, const tdb::IndexedObjects& objects, const tdb::IndexedVertices& vertices);
};



class RamResultWriter : public tdb::shapelib::BaseObjectObjectWriter
{
	using Base = tdb::shapelib::BaseObjectObjectWriter;
public:
	RamResultWriter(const sim::InteractionModel& model, const tdb::IndexedObjects& objects, const tdb::IndexedVertices& vertices);
};

} //  namespace shapelib
} // namespace sim
