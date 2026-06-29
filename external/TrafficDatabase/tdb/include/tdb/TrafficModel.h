// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "tdb_export.h"
#include <tdb/Model.h>
#include <tdb/TrafficSchemes.h>

namespace tdb {

// TODO consider ShipLink -> ShipOnLink
// TODO move the using stuff to the tdb::ShipLink etc place, better location
using IndexedShipCategories = IndexedDataTable<ShipCategoryScheme, ShipCategoryScheme::UidType>;
using IndexedShipLinks = IndexedDataTable<ShipLinkScheme, ShipLinkScheme::UidType>;
using IndexedShipCells = IndexedDataTable<ShipCellScheme, ShipCellScheme::UidType>;

struct TrafficModelDataBase
{
	DataTable<ShipCategoryScheme> shipCategoryDataBase;
	DataTable<ShipLinkScheme> shipLinkDataBase;
	DataTable<ShipCellScheme> shipCellDataBase;
};

class TDB_EXPORT TrafficModel : public BaseModel
{
public:
	TrafficModel();

	bool loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames) final;

	bool validate();

	void help() final;

	inline const IndexedShipLinks& getShipLinks() const { return m_shipLinks; }

	inline const IndexedShipCells& getShipCells() const { return m_shipCells; }

	inline const IndexedShipCategories& getShipCategories() const { return m_shipCategories; }

private:
	TrafficModelDataBase m_trafficModelDataBase;

	// Indexed tables
	IndexedShipCategories m_shipCategories;
	IndexedShipLinks m_shipLinks;
	IndexedShipCells m_shipCells;
};

} // namespace tdb
