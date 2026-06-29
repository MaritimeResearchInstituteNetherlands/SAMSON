// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/shapelib/objectWriters.h>

namespace tdb {
namespace shapelib {

WaypointObjectWriter::WaypointObjectWriter(const IndexedWaypoints& waypoints, const IndexedVertices& vertices)
	: Base(waypoints, vertices)
{
}



LinkObjectWriter::LinkObjectWriter(const IndexedLinks& links, const IndexedWaypoints& waypoints, const IndexedVertices& vertices)
	: Base(links, waypoints, vertices)
{
	addStringAttribute("uid", [&](const size_t index) -> const std::string { return m_links.get(index)->getUID(); });
	addStringAttribute("comment", [&](const size_t index) -> const std::string { return m_links.get(index)->getDescription(); });
}


// TODO make a base version
CellObjectWriter::CellObjectWriter(const IndexedCells& cells, const IndexedVertices& vertices)
	: m_cells(cells)
	, m_vertices(vertices)
{
	addStringAttribute("gridUid", [&](const size_t index) -> const std::string {return m_cells.getIndexedData()[index]->getUIDGrid(); });
}




const std::vector<std::pair<double, double>>& CellObjectWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	for (const auto& vertexIndex : m_cells.getIndexedData()[index]->getVertices()) {
		const auto vertex = m_vertices.get(vertexIndex);
		m_coordinates.push_back({ vertex->getLatitude(), vertex->getLongitude() });
	}
	return m_coordinates;
}



ObjectObjectWriter::ObjectObjectWriter(const IndexedObjects& objects, const IndexedVertices& vertices)
	: Base(objects, vertices)
{
}



ZoneObjectWriter::ZoneObjectWriter(const IndexedZones& zones, const IndexedVertices& vertices)
	: m_zones(zones)
	, m_vertices(vertices)
{
	addStringAttribute("uid", [&](const size_t index) -> const std::string { return m_zones.getIndexedData()[index]->getUID(); });
}




const std::vector<std::pair<double, double>>& ZoneObjectWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	for (const auto& vertexIndex : m_zones.getIndexedData()[index]->getVertices()) {
		const auto vertex = m_vertices.get(vertexIndex);
		m_coordinates.push_back({ vertex->getLatitude(), vertex->getLongitude() });
	}
	return m_coordinates;
}



WeatherstationObjectWriter::WeatherstationObjectWriter(const IndexedWeatherstations& weatherstations, const IndexedVertices& vertices)
	: m_weatherstations(weatherstations)
	, m_vertices(vertices)
{
	addStringAttribute("uid", [&](const size_t index) -> const std::string { return m_weatherstations.get(index)->getUID(); });
}



const std::vector<std::pair<double, double>>& WeatherstationObjectWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	const auto vertex = m_vertices.get(m_weatherstations.get(index)->getVertexUID());
	m_coordinates.push_back({ vertex->getLatitude(), vertex->getLongitude() });
	return m_coordinates;
}



ErtvObjectWriter::ErtvObjectWriter(const IndexedErtvs& ertvs, const IndexedVertices& vertices)
	: m_ertvs(ertvs)
	, m_vertices(vertices)
{
	addStringAttribute("uid", [&](const size_t index) -> const std::string { return m_ertvs.get(index)->getUID(); });
	addStringAttribute("descript", [&](const size_t index) -> const std::string { return m_ertvs.get(index)->getDescription(); });
}



const std::vector<std::pair<double, double>>& ErtvObjectWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	const auto vertex = m_vertices.get(m_ertvs.get(index)->getVertexUID());
	m_coordinates.push_back({ vertex->getLatitude(), vertex->getLongitude() });
	return m_coordinates;
}

} // namespace shapelib
} // namespace sim
