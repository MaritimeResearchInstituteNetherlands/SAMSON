// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <tdb/Model.h> // TODO does this Model have to be in the TDB? I don't think so
#include <sem/MeasureSchemes.h>

namespace sem
{
	using PilotAssistanceShip = tdb::DataTable<PilotAssistanceShipScheme>;
	using PilotAssistanceZone = tdb::DataTable<PilotAssistanceZoneScheme>;
	using IndexedPilotAssistanceShip = tdb::IndexedDataTable<PilotAssistanceShipScheme, std::string>;
	using IndexedPilotAssistanceZones = tdb::IndexedDataTable<PilotAssistanceZoneScheme, std::string>;

	class MeasureModel : public tdb::BaseModel
	{
	public:

		MeasureModel() {}

		bool loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames) final;

		void help() final;

		bool validate();

		inline const IndexedPilotAssistanceShip& getPilotAssistanceShip() const { return m_pilotAssistanceShip; }

		inline const IndexedPilotAssistanceZones& getPilotAssistanceZones() const { return m_pilotAssistanceZones; }

	private:
		// Measure database
		PilotAssistanceShip m_pilotAssistanceShipDataBase;
		PilotAssistanceZone m_pilotAssistanceZonesDataBase;

		// Indexed
		IndexedPilotAssistanceShip m_pilotAssistanceShip;
		IndexedPilotAssistanceZones m_pilotAssistanceZones;
	};
} // namespace sem

