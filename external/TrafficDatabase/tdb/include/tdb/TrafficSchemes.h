// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "util/IIdentifiable.h"
#include "util/IModel.h"
#include <boost/archive/binary_iarchive.hpp>
#include <tdb/AreaSchemes.h>

#include <string>

namespace sem {
	class SamsonExposureModel; //tmp hack
}
namespace tdb {
/**
	* @class ShipCategory
	* @brief Defines a ship category where ships are categorized based on their type and size.
	*
	*/

class ShipCategoryScheme : public IIdentifiable {
public:
	using UidType = std::string;

	/**
		* @brief Function to get the unique identifier of the ship category.
		*/
	inline const std::string getUID() const override { return m_uid; }

	/**
		* @brief Function to serialize and deserialize the ship category object.
		*
		* @param[in] ar The archive to serialize and deserialize the area
		* @param[in] version The version of the serialization (not used)
		*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("type", m_type);
		ar& boost::serialization::make_nvp("length", m_length);
		ar& boost::serialization::make_nvp("width", m_width);
		ar& boost::serialization::make_nvp("height", m_height);
		ar& boost::serialization::make_nvp("draft", m_draft);
		ar& boost::serialization::make_nvp("default_speed", m_defaultSpeed);
		ar& boost::serialization::make_nvp("description", m_description);
	}

	inline const double getLength() const { return m_length; }

	inline const double getWidth() const { return m_width; }

	inline const double getHeight() const { return m_height; }

	inline const double getDraft() const { return m_draft; }

	inline const double getDefaultSpeed() const { return m_defaultSpeed; }

private:
	UidType m_uid;
	//TBD: Use type and size to define details of a ship category in data tables.
	// Type::UidType and Size::Uid to access those tables and get the details.
	// std::string m_ShipCatData;
	std::string m_type;
	int m_size;
	double m_length;
	double m_width;
	double m_height;
	double m_draft;
	double m_defaultSpeed;
	std::string m_description;

	friend sem::SamsonExposureModel; // tmp hack
};



class ShipCellScheme : public IIdentifiable
{
public:
	ShipCellScheme() {}

	inline const std::string getUID() const override { return m_uid; }

	/**
	* @brief Function to serialize and deserialize the ship link object.
	*
	* @param[in] ar The archive to serialize and deserialize the ship link object
	* @param[in] version The version of the serialization (not used)
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("ship_category_uid", m_shipCategoryUid);
		ar& boost::serialization::make_nvp("cell_uid", m_cellUid);
		ar& boost::serialization::make_nvp("shipcelldata_uid", m_shipcelldataUid);
	}

	inline const auto& getShipCategoryUid() const { return m_shipCategoryUid; }

	inline const auto& getCellUid() const { return m_cellUid; }

	inline const auto& getShipCellDataUid() const { return m_shipcelldataUid; }

	using UidType = std::string;
private:
	UidType m_uid;
	ShipCategoryScheme::UidType m_shipCategoryUid;
	CellScheme::UidType m_cellUid;
	UidType m_shipcelldataUid;
};



/**
	* @class ShipLink
	* @brief Defines a ship (category) assigned to a link with a count of ships.
	*/
class ShipLinkScheme : public IIdentifiable
{
public:
	ShipLinkScheme() {}

	inline const std::string getUID() const override { return m_uid; }

	/**
		* @brief Function to serialize and deserialize the ship link object.
		*
		* @param[in] ar The archive to serialize and deserialize the ship link object
		* @param[in] version The version of the serialization (not used)
		*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("ship_category_uid", m_shipCategoryUid);
		ar& boost::serialization::make_nvp("link_uid", m_linkUid);
		ar& boost::serialization::make_nvp("shipcelldata_uid", m_shiplinkdataUid);
	}

	inline const auto& getShipCategoryUid() const { return m_shipCategoryUid; }

	inline const auto& getLinkUid() const { return m_linkUid; }

	inline const auto& getShipLinkDataUid() const { return m_shiplinkdataUid; }

	using UidType = std::string;
private:
	UidType m_uid;
	ShipCategoryScheme::UidType m_shipCategoryUid;
	LinkScheme::UidType m_linkUid;
	UidType m_shiplinkdataUid;
};

} // namespace tdb
