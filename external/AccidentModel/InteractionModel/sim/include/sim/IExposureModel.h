// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
// Copyright (c), MARIN, Maritime Research Institute Netherlands. All rights reserved.
// License: pending

#pragma once

#include "sim_export.h"
#include <tdb/Model.h>
#include <tdb/TrafficDataBase.h>
#include <sim/InteractionTypes.h>
#include <sim/Link.h>
#include <sim/Cell.h>
#include <sim/Object.h>
#include <sml/AngularDistributionFunction.h>
#include <map>

namespace sim
{
class SIM_EXPORT ShipLinkData : public tdb::IIdentifiable
{
public:
	ShipLinkData() {}

	inline const std::string getUID() const override { return m_uid; }

	inline const auto& getShipLinkUid() const { return m_shipLink_uid; }

	/**
	* @brief Function to serialize and deserialize the ship link object.
	*
	* @param[in] ar The archive to serialize and deserialize the ship link object
	* @param[in] version The version of the serialization (not used)
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(m_uid);
		ar& BOOST_SERIALIZATION_NVP(m_shipLink_uid);
	}

	using UidType = std::string;
private:
	UidType m_uid;
	tdb::ShipLinkScheme::UidType m_shipLink_uid;
};



class SIM_EXPORT ShipCellData : public tdb::IIdentifiable
{
public:
	ShipCellData() {}

	inline const std::string getUID() const override { return m_uid; }

	inline const auto& getShipCellUid() const { return m_shipCell_uid; }

	/**
	* @brief Function to serialize and deserialize the ship link object.
	*
	* @param[in] ar The archive to serialize and deserialize the ship link object
	* @param[in] version The version of the serialization (not used)
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(m_uid);
		ar& BOOST_SERIALIZATION_NVP(m_shipCell_uid);
	}

	using UidType = std::string;
private:
	UidType m_uid;
	tdb::ShipLinkScheme::UidType m_shipCell_uid;
};



class SIM_EXPORT IExposureModel : public tdb::BaseModel
{
public:
	virtual void setup(const tdb::AreaModel& areaModel, const tdb::TrafficModel& trafficModel) = 0;

	// TODO perhaps move this to trafficDataBase? isntead of tdb::BaseModel, tdb::BaseExposureModel
	virtual const size_t getShipLinkDataIndex(const sim::ShipLinkData::UidType& uid) const = 0;

	virtual const size_t getShipCellDataIndex(const sim::ShipCellData::UidType& uid) const = 0;

	virtual std::unique_ptr<sim::Link> createLink(const size_t& linkIndex, const tdb::Data<tdb::LinkScheme>& linkScheme, const tdb::TrafficDataBase& trafficDataBase) const = 0;
	
	virtual std::unique_ptr<sim::Cell> createCell(const size_t& cellIndex, const tdb::Data<tdb::CellScheme>& cellScheme, const tdb::TrafficDataBase& trafficDataBase) const = 0;
	
	virtual std::unique_ptr<sim::Object> createObject(const size_t& objectIndex, const tdb::Data<tdb::ObjectScheme>& objectScheme, const std::vector<sml::Coordinate2D>& coordinates) const = 0;

	virtual std::unique_ptr<sim::LinkLink> createLinkLink(const sim::Link& linkA, const sim::Link& linkB, const tdb::TrafficDataBase& trafficDataBase) const = 0;

	virtual std::unique_ptr<sim::LinkCell> createLinkCell(const sim::Link& link, const sim::Cell& cell, const tdb::TrafficDataBase& trafficDatabase) const = 0;

	virtual std::unique_ptr<sim::LinkObject> createLinkObject(const sim::Link& link, const tdb::TrafficDataBase& trafficDataBase, const tdb::WeatherstationScheme& wss) const = 0;

	virtual std::unique_ptr<sim::CellObject> createCellObject(const sim::Cell& cell, const tdb::TrafficDataBase& trafficDataBase, const tdb::WeatherstationScheme& wss) const = 0;

	virtual bool isLinkLinkPair(const sim::Link& linkA, const sim::Link& linkB) const = 0;

	virtual bool isLinkCellPair(const sim::Link& link, const sim::Cell& cell) const = 0;

	virtual bool isLinkObjectPair(const size_t& linkIndex, const size_t& objectIndex) const = 0;

	virtual bool isCellObjectPair(const size_t& cellIndex, const size_t& objectIndex) const = 0;
};
} // namespace sim
