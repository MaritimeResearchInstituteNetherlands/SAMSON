// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/AreaSchemes.h>

namespace tdb {

VertexScheme::VertexScheme()
	: m_uid(0)
	, m_lat(0)
	, m_lon(0)
{
}



VertexScheme::VertexScheme(UidType uid, double lat, double lon)
	: m_uid(uid)
	, m_lat(lat)
	, m_lon(lon)
{
}



VertexScheme::VertexScheme(const VertexScheme& other)
	: m_uid(other.m_uid)
	, m_lat(other.m_lat)
	, m_lon(other.m_lon)
{
}



NodeScheme::NodeScheme()
	: m_uid("")
	, m_vertex(0)
	, m_description("")
{

}



NodeScheme::NodeScheme(UidType uid, VertexScheme::UidType vertexUID, std::string description)
	: m_uid(uid)
	, m_vertex(vertexUID)
	, m_description(description)
{

}



NodeScheme::NodeScheme(const NodeScheme& other)
	: m_uid(other.m_uid)
	, m_vertex(other.m_vertex)
	, m_description(other.m_description)
{

}



WaypointScheme::WaypointScheme()
	: base()
	, m_radius(0.0)
{
}



WaypointScheme::WaypointScheme(std::string uid, tdb::VertexScheme::UidType vertexUID, double radius, std::string description)
	: base(uid, vertexUID, description)
	, m_radius(radius)
{
}



WaypointScheme::WaypointScheme(const WaypointScheme& other)
	: base(other)
	, m_radius(other.m_radius)
{
}



LinkScheme::LinkScheme()
	: m_uid("")
	, m_start("")
	, m_end("")
	, m_description("")
{
}



LinkScheme::LinkScheme(std::string uid, WaypointScheme::UidType start, WaypointScheme::UidType end, std::string description)
	: m_uid(uid)
	, m_start(start)
	, m_end(end)
	, m_description(description)
{
}



LinkScheme::LinkScheme(const LinkScheme& other)
	: m_uid(other.m_uid)
	, m_start(other.m_start)
	, m_end(other.m_end)
	, m_description(other.m_description)
{
}



CellScheme::CellScheme()
	: m_uid("")
	, m_vertices()
	, m_gridUid("")
	, m_description("")
{
}



CellScheme::CellScheme(const std::string& uid, std::vector<VertexScheme::UidType>& vertices, const std::string& uidGrid, const std::string& description)
	: m_uid(uid)
	, m_vertices(std::move(vertices))
	, m_gridUid(uidGrid)
	, m_description(description)
{
}



CellScheme::CellScheme(const CellScheme& other)
	: m_uid(other.m_uid)
	, m_vertices(other.m_vertices)
	, m_gridUid(other.m_gridUid)
	, m_description(other.m_description)
{
}



ObjectScheme::ObjectScheme()
	: m_uid("")
	, m_type("")
	, m_vertices()
	, m_elevation(0)
	, m_description("")
{
}



ObjectScheme::ObjectScheme(std::string uid, std::string type, std::vector<VertexScheme::UidType>& vertices, std::string description)
	: m_uid(uid)
	, m_type(type)
	, m_vertices(vertices)
	, m_description(description)
{
}



ObjectScheme::ObjectScheme(const ObjectScheme& other)
	: m_uid(other.m_uid)
	, m_type(other.m_type)
	, m_vertices(other.m_vertices)
	, m_description(other.m_description)
{
}



ZoneScheme::ZoneScheme()
	: m_uid("")
	, m_vertices()
	, m_description("")
{
}



ZoneScheme::ZoneScheme(std::string uid, std::string type, std::vector<VertexScheme::UidType>& vertices, std::string description)
	: m_uid(uid)
	, m_vertices(vertices)
	, m_description(description)
{
}



ZoneScheme::ZoneScheme(const ZoneScheme& other)
	: m_uid(other.m_uid)
	, m_vertices(other.m_vertices)
	, m_description(other.m_description)
{
}



WeatherstationScheme::WeatherstationScheme()
	: base()
	, m_windTableUid("")
{
}



WeatherstationScheme::WeatherstationScheme(std::string uid, VertexScheme::UidType vertex, std::string description, std::string windTable)
	: base(uid, vertex, description)
	, m_windTableUid(windTable)
{
}



WeatherstationScheme::WeatherstationScheme(const WeatherstationScheme& other)
	: base(other)
	, m_windTableUid(other.m_windTableUid)
{
}



ErtvScheme::ErtvScheme()
	: base()
{
}



ErtvScheme::ErtvScheme(std::string uid, VertexScheme::UidType vertex, std::string description)
	: base(uid, vertex, description)
{
}



ErtvScheme::ErtvScheme(const ErtvScheme& other)
	: base(other)
{
}



WindstrengthScheme::WindstrengthScheme()
	: m_uid(0)
	, m_tableUid("")
	, m_scale(0)
	, m_probability(0)
	, m_waveHeight(0)
	, m_directionTableUid("")
{
}



WindstrengthScheme::WindstrengthScheme(size_t uid, std::string tableUid, int scale, double probability, double waveHeight, std::string directionTableUid)
	: m_uid(uid)
	, m_tableUid(tableUid)
	, m_scale(scale)
	, m_probability(probability)
	, m_waveHeight(waveHeight)
	, m_directionTableUid(directionTableUid)
{
}



WindstrengthScheme::WindstrengthScheme(const WindstrengthScheme& other)
	: m_uid(other.m_uid)
	, m_tableUid(other.m_tableUid)
	, m_scale(other.m_scale)
	, m_probability(other.m_probability)
	, m_waveHeight(other.m_waveHeight)
	, m_directionTableUid(other.m_directionTableUid)
{
}



WinddirectionScheme::WinddirectionScheme()
	: m_uid(0)
	, m_tableUid("")
	, m_angle(0.0)
	, m_probability(0.0)
{
}



WinddirectionScheme::WinddirectionScheme(size_t uid, std::string tableUid, double angle, double probability)
	: m_uid(0)
	, m_tableUid(tableUid)
	, m_angle(angle)
	, m_probability(probability)
{
}



WinddirectionScheme::WinddirectionScheme(const WinddirectionScheme& other)
	: m_uid(other.m_uid)
	, m_tableUid(other.m_tableUid)
	, m_angle(other.m_angle)
	, m_probability(other.m_probability)
{
}



CurrentScheme::CurrentScheme()
	: m_uid(0)
	, m_tableUid("")
	, m_velocity(0.0)
	, m_angle(0.0)
	, m_probability(0.0)
{
}



TideScheme::TideScheme()
	: m_uid(0)
	, m_tableUid("")
	, m_amplitude(0.0)
	, m_angularFrequency(0.0)
	, m_phase(0.0)
{
}
} // namespace tdb
