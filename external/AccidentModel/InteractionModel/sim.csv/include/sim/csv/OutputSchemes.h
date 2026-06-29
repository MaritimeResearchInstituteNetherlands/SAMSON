// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "util/IIdentifiable.h" // TODO move this to a standalone database library
#include <tdb/AreaSchemes.h>
#include <tdb/TrafficSchemes.h>
#include <sim/ShipShipEntity.h>

namespace sim
{
namespace csv
{

class LoaderSaver;  // Forward declaration for friend class

class BaseShipShipScheme : public tdb::IIdentifiable
{
public:
	BaseShipShipScheme();

	static constexpr std::string getHeader() { return std::string("meeting_index, shipA_uid, shipB_uid, exposures, events, collisions"); }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("meeting_index", m_meeting.index);
		ar& boost::serialization::make_nvp("trafficA_uid", m_trafficAUID);
		ar& boost::serialization::make_nvp("trafficB_uid", m_trafficBUID);
		ar& boost::serialization::make_nvp("exposures", m_meeting.exposures);
		ar& boost::serialization::make_nvp("struck_chance_i", m_meeting.struckChance_i);
		ar& boost::serialization::make_nvp("struck_chance_j", m_meeting.struckChance_j);
		ar& boost::serialization::make_nvp("events_i", m_meeting.events_i);
		ar& boost::serialization::make_nvp("events_j", m_meeting.events_j);
		ar& boost::serialization::make_nvp("collisions_i", m_meeting.collisions_i);
		ar& boost::serialization::make_nvp("collisions_j", m_meeting.collisions_j);
	}

	inline size_t getMeetingIndex() const { return m_meeting.index; }

private:
	std::string m_trafficAUID;
	std::string m_trafficBUID;
	ShipShipMeeting m_meeting;

	friend class LoaderSaver;
};



class BaseShipObjectScheme : public tdb::IIdentifiable
{
public:
	BaseShipObjectScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("meeting_index", m_meetingIndex);
		ar& boost::serialization::make_nvp("trafficUID", m_trafficUID);
		ar& boost::serialization::make_nvp("objectUID", m_objectUID);
		ar& boost::serialization::make_nvp("driftExposure", m_driftExposure);
		ar& boost::serialization::make_nvp("ramExposure", m_ramExposure);
		ar& boost::serialization::make_nvp("driftEvents", m_driftEvents);
		ar& boost::serialization::make_nvp("ramEvents", m_ramEvents);
		ar& boost::serialization::make_nvp("driftCollisions", m_driftCollisions);
		ar& boost::serialization::make_nvp("ramCollisions", m_ramCollisions);
	}

	inline const size_t getMeetingIndex() const { return m_meetingIndex; }

private:
	size_t m_meetingIndex;
	std::string m_trafficUID;
	tdb::ShipCategoryScheme::UidType m_objectUID;
	double m_driftExposure;
	double m_ramExposure;
	double m_driftEvents;
	double m_ramEvents;
	double m_driftCollisions;
	double m_ramCollisions;

	friend class LoaderSaver;
};



class LinkScheme : public BaseShipShipScheme
{
	using base = BaseShipShipScheme;
public:
	using UidType = std::string;

	LinkScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("link_uid", m_linkUID);
		ar& boost::serialization::make_nvp("link_length", m_linkLength);
		base::serialize(ar, version);
	}
	inline const std::string getUID() const override { return m_linkUID + "_" + std::to_string(getMeetingIndex()); }

	inline const tdb::LinkScheme::UidType getLinkUID() const { return m_linkUID; }

private:
	tdb::LinkScheme::UidType m_linkUID;
	double m_linkLength;

	friend class LoaderSaver;
};



class LinkLinkScheme : public BaseShipShipScheme
{
	using base = BaseShipShipScheme;
public:
	using UidType = std::string;

	LinkLinkScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("linkA_uid", m_linkAUID);
		ar& boost::serialization::make_nvp("linkB_uid", m_linkBUID);
		ar& boost::serialization::make_nvp("angle", m_angle);
		base::serialize(ar, version);
	}

	inline const std::string getUID() const override { return m_linkAUID + "_" + m_linkBUID + "_" + std::to_string(getMeetingIndex()); }

	inline const tdb::LinkScheme::UidType getLinkAUID() const { return m_linkAUID; }

	inline const tdb::LinkScheme::UidType getLinkBUID() const { return m_linkBUID; }

private:
	tdb::LinkScheme::UidType m_linkAUID;
	tdb::LinkScheme::UidType m_linkBUID;
	double m_angle;

	friend class LoaderSaver;
};



class LinkObjectScheme : public BaseShipObjectScheme
{
	using base = BaseShipObjectScheme;
public:
	using UidType = std::string;

	LinkObjectScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("link_uid", m_linkUID);
		base::serialize(ar, version);
	}
	inline const std::string getUID() const override { return m_linkUID + "_" + std::to_string(getMeetingIndex()); }

	inline const tdb::LinkScheme::UidType getLinkUID() const { return m_linkUID; }

private:
	tdb::LinkScheme::UidType m_linkUID;

	friend class LoaderSaver;
};



class CellScheme : public BaseShipShipScheme
{
	using base = BaseShipShipScheme;
public:
	using UidType = std::string;

	CellScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("cell_uid", m_cellUID);
		ar& boost::serialization::make_nvp("grid_uid", m_gridUID);
		ar& boost::serialization::make_nvp("area", m_area);
		base::serialize(ar, version);
	}

	inline const std::string getUID() const override { return m_cellUID + "_" + std::to_string(getMeetingIndex()); }
private:
	tdb::CellScheme::UidType m_cellUID;
	tdb::CellScheme::UidType m_gridUID;
	double m_area;

	friend class LoaderSaver;
};



class CellObjectScheme : public BaseShipObjectScheme
{
	using base = BaseShipObjectScheme;
public:
	using UidType = std::string;

	CellObjectScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("cell_uid", m_cellUID);
		base::serialize(ar, version);
	}
	inline const std::string getUID() const override { return m_cellUID + "_" + std::to_string(getMeetingIndex()); }

	inline const tdb::LinkScheme::UidType getLinkUID() const { return m_cellUID; }

private:
	tdb::CellScheme::UidType m_cellUID;

	friend class LoaderSaver;
};



class LinkCellScheme : public BaseShipShipScheme
{
	using base = BaseShipShipScheme;
public:
	using UidType = std::string;

	LinkCellScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("link_uid", m_linkUID);
		ar& boost::serialization::make_nvp("cell_uid", m_cellUID);
		ar& boost::serialization::make_nvp("grid_uid", m_gridUID);
		base::serialize(ar, version);
	}

	inline const std::string getUID() const override { return m_linkUID + "_" + m_cellUID + "_" + std::to_string(getMeetingIndex()); }

private:
	tdb::CellScheme::UidType m_linkUID;
	tdb::CellScheme::UidType m_cellUID;
	tdb::CellScheme::UidType m_gridUID;
	double m_area;

	friend class LoaderSaver;
};



} // namespace csv
} // namespace sim
