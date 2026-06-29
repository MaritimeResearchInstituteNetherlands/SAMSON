// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/shapelib/baseObjectWriters.h>

namespace tdb {
namespace shapelib {

BaseWaypointObjectWriter::BaseWaypointObjectWriter(const IndexedWaypoints& waypoints, const IndexedVertices& vertices)
	: m_waypoints(waypoints)
	, m_vertices(vertices)
{
	addStringAttribute("uid", [&](const size_t index) -> const std::string { return m_waypoints.get(index)->getUID(); });
}



const std::vector<std::pair<double, double>>& BaseWaypointObjectWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	const auto vertex = m_vertices.get(m_waypoints.get(index)->getVertexUID());
	m_coordinates.push_back({ vertex->getLatitude(), vertex->getLongitude() });
	return m_coordinates;
}



BaseObjectObjectWriter::BaseObjectObjectWriter(const IndexedObjects& objects, const IndexedVertices& vertices)
	: m_objects(objects)
	, m_vertices(vertices)
{
	addStringAttribute("uid", [&](const size_t index) -> const std::string { return m_objects.getIndexedData()[index]->getUID(); });
}



BaseLinkObjectWriter::BaseLinkObjectWriter(const IndexedLinks& links, const IndexedWaypoints& waypoints, const IndexedVertices& vertices)
	: m_links(links)
	, m_waypoints(waypoints)
	, m_vertices(vertices)
{
}



const std::vector<std::pair<double, double>>& BaseLinkObjectWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	const auto indexStart = m_waypoints.getIndex(m_links.get(index)->getStart());
	const auto indexEnd = m_waypoints.getIndex(m_links.get(index)->getEnd());
	const auto vertexStart = m_vertices.get(m_waypoints.get(indexStart)->getVertexUID());
	const auto vertexEnd = m_vertices.get(m_waypoints.get(indexEnd)->getVertexUID());
	m_coordinates.push_back({ vertexStart->getLatitude(), vertexStart->getLongitude() });
	m_coordinates.push_back({ vertexEnd->getLatitude(), vertexEnd->getLongitude() });
	return m_coordinates;
}



const std::vector<std::pair<double, double>>& BaseObjectObjectWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	for (const auto& vertexIndex : m_objects.getIndexedData()[index]->getVertices()) {
		const auto vertex = m_vertices.get(vertexIndex);
		m_coordinates.push_back({ vertex->getLatitude(), vertex->getLongitude() });
	}
	return m_coordinates;
}

} // namespace shapelib
} // namespace tdb
