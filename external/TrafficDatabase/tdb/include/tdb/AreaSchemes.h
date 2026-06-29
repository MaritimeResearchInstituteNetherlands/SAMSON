// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file AreaSchames.h
 * @brief This file contains the declaration of the area database schemes
 */

#pragma once

#include "util/IIdentifiable.h"

#include <boost/serialization/vector.hpp>

#include <memory>
#include <string>
#include <vector>


namespace tdb {

class VertexScheme : public IIdentifiable
{
public:
	using UidType = size_t;

	VertexScheme();

	VertexScheme(UidType uid, double lat, double lon);

	VertexScheme(const VertexScheme& other);

	inline const std::string getUID() const final { return std::to_string(m_uid); }

	inline const size_t getIndex() const { return m_uid; }

	inline void setIndex(size_t index) { m_uid = index; }

	inline const double getLatitude() const { return m_lat; }

	inline void setLattitude(const double lat) { m_lat = lat; }

	inline const double getLongitude() const { return m_lon; }

	inline void setLongitude(const double lon) { m_lon = lon; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("lat", m_lat);
		ar& boost::serialization::make_nvp("lon", m_lon);
	}
private:
	UidType m_uid;
	double m_lat;
	double m_lon;
};



// This is a scheme with has only one vertex
class NodeScheme : public IIdentifiable
{
public:
	using UidType = std::string;

	NodeScheme();

	NodeScheme(UidType uid, VertexScheme::UidType vertexUID, std::string description);

	NodeScheme(const NodeScheme& other);

	virtual ~NodeScheme(){}

	inline const std::string getUID() const final { return m_uid; }

	inline const VertexScheme::UidType getVertexUID() const { return m_vertex; }
	
	inline void setVertexUID(VertexScheme::UidType vertex) { m_vertex = vertex; }
	
	inline const std::string getDescription() const { return m_description; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("vertex_uid", m_vertex);
		ar& boost::serialization::make_nvp("description", m_description);
	}

private:
	UidType m_uid;
	VertexScheme::UidType m_vertex;
	std::string m_description;
};



class WaypointScheme : public NodeScheme
{
	using base = NodeScheme;
public:
	using UidType = std::string;

	WaypointScheme();

	WaypointScheme(std::string uid, VertexScheme::UidType vertexUID, double rangeOfInfluence, std::string description);

	WaypointScheme(const WaypointScheme& other);

	virtual ~WaypointScheme() {}

	inline double getRadius() const { return m_radius; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<NodeScheme>(*this);
		ar& boost::serialization::make_nvp("radius", m_radius);
	}

private:
	double m_radius;
};


/**
* @class Link
* @brief Defines a link between two waypoints.
*/
class LinkScheme : public IIdentifiable
{
public:
	using UidType = std::string;

	LinkScheme();

	LinkScheme(std::string uid, WaypointScheme::UidType start, WaypointScheme::UidType end, std::string description);

	LinkScheme(const LinkScheme& other);

	inline const std::string getUID() const final { return m_uid; }

	inline const WaypointScheme::UidType getStart() { return m_start; }

	inline const WaypointScheme::UidType getEnd() { return m_end; }

	inline const std::string& getDescription() const { return m_description; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("start", m_start);
		ar& boost::serialization::make_nvp("end", m_end);
		ar& boost::serialization::make_nvp("description", m_description);
	}

private:
	UidType m_uid;
	WaypointScheme::UidType m_start;
	WaypointScheme::UidType m_end;
	std::string m_description;
};



class CellScheme : public IIdentifiable
{
public:
	using UidType = std::string;

	CellScheme();

	CellScheme(const std::string& uid, std::vector<VertexScheme::UidType>& vertices, const std::string& uidGrid, const std::string& description);

	CellScheme(const CellScheme& other);

	inline const std::string getUID() const final { return m_uid; }

	inline const std::vector<VertexScheme::UidType>& getVertices() const { return m_vertices; }

	inline void setVertices(const std::vector<VertexScheme::UidType>& vertices) { m_vertices = vertices; }

	inline const std::string& getUIDGrid() const { return m_gridUid; }

	inline const std::string& getDescription() const { return m_description; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("vertex_uids", m_vertices);
		ar& boost::serialization::make_nvp("grid_uid", m_gridUid);
		ar& boost::serialization::make_nvp("description", m_description);
	}

private:
	UidType m_uid;
	std::vector<VertexScheme::UidType> m_vertices;
	std::string m_gridUid;
	std::string m_description;
};



class ObjectScheme : public IIdentifiable
{
public:
	using UidType = std::string;

	ObjectScheme();

	ObjectScheme(std::string uid, std::string type, std::vector<VertexScheme::UidType>& vertices, std::string description);

	ObjectScheme(const ObjectScheme& other);

	inline const std::string getUID() const override { return m_uid; }

	inline const std::string& getType() const { return m_type; }

	inline const std::vector<VertexScheme::UidType>& getVertices() const { return m_vertices; }

	inline void setVertices(const std::vector<VertexScheme::UidType>& vertices) { m_vertices = vertices; }

	inline const double getElevation() const { return m_elevation; }

	inline const std::string& getDescription() const { return m_description; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("type", m_type);
		ar& boost::serialization::make_nvp("vertex_uids", m_vertices);
		ar& boost::serialization::make_nvp("elevation", m_elevation);
		ar& boost::serialization::make_nvp("description", m_description);
	}

private:
	UidType m_uid;
	std::string m_type;
	std::vector<VertexScheme::UidType> m_vertices;
	double m_elevation;
	std::string m_description;
};



class ZoneScheme : public IIdentifiable
{
public:
	using UidType = std::string;

	ZoneScheme();

	ZoneScheme(std::string uid, std::string type, std::vector<VertexScheme::UidType>& vertices, std::string description);

	ZoneScheme(const ZoneScheme& other);

	inline const std::string getUID() const override { return m_uid; }

	inline const std::vector<VertexScheme::UidType>& getVertices() const { return m_vertices; }

	inline void setVertices(const std::vector<VertexScheme::UidType>& vertices) { m_vertices = vertices; }

	inline const std::string& getDescription() const { return m_description; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("vertex_uids", m_vertices);
		ar& boost::serialization::make_nvp("description", m_description);
	}

private:
	UidType m_uid;
	std::vector<VertexScheme::UidType> m_vertices;
	std::string m_description;
};



class WeatherstationScheme : public NodeScheme
{
	using base = NodeScheme;
public:
	using UidType = std::string;

	WeatherstationScheme();

	WeatherstationScheme(std::string uid, VertexScheme::UidType vertex, std::string description, std::string windTable);

	WeatherstationScheme(const WeatherstationScheme& other);

	inline const std::string& getWindTableUid() const { return m_windTableUid; }

	inline const std::string& getCurrentTableUid() const { return m_currentTableUid; }

	inline const std::string& getTideTableUid() const { return m_tideTableUid; }

	inline const double getTideAngle() const { return m_tideAngle; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<NodeScheme>(*this);
		ar& boost::serialization::make_nvp("windstrength_table_uid", m_windTableUid);
		ar& boost::serialization::make_nvp("current_table_uid", m_currentTableUid);
		ar& boost::serialization::make_nvp("tide_table_uid", m_tideTableUid);
		ar& boost::serialization::make_nvp("tide_angle", m_tideAngle);
	}
private:
	std::string m_windTableUid;
	std::string m_currentTableUid;
	std::string m_tideTableUid;
	double m_tideAngle;
};



class ErtvScheme : public NodeScheme
{
	using base = NodeScheme;
public:
	using UidType = std::string;

	ErtvScheme();

	ErtvScheme(std::string uid, VertexScheme::UidType vertex, std::string description);

	ErtvScheme(const ErtvScheme& other);

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::base_object<NodeScheme>(*this);
	}
};



class WinddirectionScheme : public IIdentifiable
{
public:
	using UidType = size_t;

	WinddirectionScheme();

	WinddirectionScheme(size_t uid, std::string tableUid, double angle, double probability);

	WinddirectionScheme(const WinddirectionScheme& other);

	inline const std::string getUID() const override { return std::to_string(m_uid); }

	inline const size_t getEntry() const { return m_uid; }

	inline const std::string& getTableUid() const { return m_tableUid; }

	inline const double getAngle() const { return m_angle; }

	inline const double getProbability() const { return m_probability; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("table_uid", m_tableUid);
		ar& boost::serialization::make_nvp("angle", m_angle);
		ar& boost::serialization::make_nvp("probabilty", m_probability);
	}
private:
	UidType m_uid;
	std::string m_tableUid;
	double m_angle;
	double m_probability;
};



class WindstrengthScheme : public IIdentifiable
{
public:
	using UidType = size_t;

	WindstrengthScheme();

	WindstrengthScheme(size_t uid, std::string tableUid, int beaufortNumber, double probability, double waveHeight, std::string directionTableUid);

	WindstrengthScheme(const WindstrengthScheme& other);

	inline const std::string getUID() const override { return std::to_string(m_uid); }

	inline const size_t getEntry() const { return m_uid; }

	inline const std::string& getTableUid() const { return m_tableUid; }

	inline const int getScale() const { return m_scale; }

	inline const double getProbability() const { return m_probability; }

	inline const double getWaveHeight() const { return m_waveHeight; }

	inline const std::string& getDirectionTableUid() const { return m_directionTableUid; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("table_uid", m_tableUid);
		ar& boost::serialization::make_nvp("scale", m_scale);
		ar& boost::serialization::make_nvp("probability", m_probability);
		ar& boost::serialization::make_nvp("wave_height", m_waveHeight);
		ar& boost::serialization::make_nvp("winddirection_table_uid", m_directionTableUid);
	}
private:
	UidType m_uid;
	std::string m_tableUid;
	int m_scale;
	double m_probability;
	double m_waveHeight;
	std::string m_directionTableUid;

};



class CurrentScheme : public IIdentifiable
{
public:
	using UidType = size_t;

	CurrentScheme();

	inline const std::string getUID() const override { return std::to_string(m_uid); }

	inline const std::string& getTableUid() const { return m_tableUid; }

	inline const double getVelocity() const { return m_velocity; }

	inline const double getAngle() const { return m_angle; }

	inline const double getProbability() const { return m_probability; }

	template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("table_uid", m_tableUid);
		ar& boost::serialization::make_nvp("velocity", m_velocity);
		ar& boost::serialization::make_nvp("angle", m_angle);
		ar& boost::serialization::make_nvp("probability", m_probability);

	}
private:
	UidType m_uid;
	std::string m_tableUid;
	double m_velocity;
	double m_angle;
	double m_probability;
};



class TideScheme : public IIdentifiable
{
public:
	using UidType = size_t;

	TideScheme();

	inline const std::string getUID() const override { return std::to_string(m_uid); }

	inline const size_t getEntry() const { return m_uid; }

	inline const std::string& getTableUid() const { return m_tableUid; }

	inline const double getAmplitude() const { return m_amplitude; }

	inline const double getAngularFrequency() const { return m_angularFrequency; }

	inline const double getPhase() const { return m_phase; }

	template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("table_uid", m_tableUid);
		ar& boost::serialization::make_nvp("amplitude", m_amplitude);
		ar& boost::serialization::make_nvp("angular_frequency", m_angularFrequency);
		ar& boost::serialization::make_nvp("phase", m_phase);
	}
private:
	UidType m_uid;
	std::string m_tableUid;
	double m_amplitude;
	double m_angularFrequency;
	double m_phase;
};
} // namespace tdb  
