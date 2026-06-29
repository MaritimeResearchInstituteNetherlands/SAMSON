// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sem_export.h"
#include <tdb/Model.h>
#include <sml/AngleBounds.h>
#include <sem/DataSchemes.h>
#include <numbers>

namespace sem
{

class DataModel : public tdb::BaseModel
{
public:
	using IndexedShipLinkData = tdb::IndexedDataTable<sem::ShipLinkDataScheme, sem::ShipLinkDataScheme::UidType>;
	using IndexedShipCellData = tdb::IndexedDataTable<sem::ShipCellDataScheme, sem::ShipCellDataScheme::UidType>;

	DataModel() {}

	bool loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames) final;

	bool validate();

	void help() final;

	inline const auto& getIndexedShipLinkData() const { return m_shipLinkData; }

	inline const auto& getIndexedShipCellData() const { return m_shipCellData; }

private:
	tdb::DataTable<ShipLinkDataScheme> m_shipLinkDataBase;
	tdb::DataTable<ShipCellDataScheme> m_shipCellDataBase;

	// Indexed tables for fast reference
	IndexedShipLinkData m_shipLinkData;
	IndexedShipCellData m_shipCellData;
};



class GeneralModelData
{
public:
	// Note: This is not a good pattern if you have a single process that is loading different models...
	static inline GeneralModelData& get()
	{
		static GeneralModelData instance;
		return instance;
	}

	inline double getWindSpeed(const size_t scale) const { return scale < 13 ? m_windSpeeds[scale] : -1.0; }

	inline double getPeakPeriod(const size_t scale) const { return scale < 13 ? m_peakPeriod[scale] : -1.0; }

	inline double getAnchorFailure(const size_t scale) const { return scale < 13 ? m_pAnchorFailure[scale] : -1.0; }

	static consteval double g() { return 9.813; }

	static consteval double rhoWater() { return 1025.0; }

	static consteval double rhoAir() { return 1.23; }

	static consteval double hullDragCoefficient() { return 0.8; }

	static consteval double windDragCoefficient() { return 0.9;  }

	static consteval double passingDistanceobjectArea() { return 40000; }

	static consteval double minimumPassingDistance() { return 500; }
	
	static consteval double maxSubLinkBinSize() { return 200.0; }

	static consteval double maxSubCellGridSize() { return 200.0; }

	static consteval size_t numSubCellLink() { return 20; }

	static consteval size_t numTideSamples() { return 10; }

	static consteval double ramSigma() { return 0.2529; }
	
	static consteval double ramRadius() { return 20000; }

	static consteval size_t numAngularBins() { return 64; }

	static consteval double maxAngularBinSize() { return 2.0 * std::numbers::pi / numAngularBins(); } // TODO perhaps make this number equal to the resulting minAngleBinSize used in object discretisation

	const sml::AngleBounds& localRamBounds() { return m_localRamBounds; }

private:
	GeneralModelData();

	~GeneralModelData() = default;

	std::vector<double> m_windSpeeds;
	std::vector<double> m_peakPeriod;
	std::vector<double> m_pAnchorFailure;
	sml::AngleBounds m_localRamBounds;
	// TODO significant wave height from here or from csv?
};
} // namespace sem


