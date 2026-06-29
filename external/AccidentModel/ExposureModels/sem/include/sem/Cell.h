// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sem_export.h"
#include <sem/ExposureModel.h>
#include <sim/InteractionTypes.h>
#include <sml/Polygon.h>
#include <tdb/TrafficDataBase.h>

namespace sem
{
class SEM_EXPORT Cell : public sim::Cell
{
	using base = sim::Cell;
public:
	Cell(const size_t& index, const tdb::Data<tdb::CellScheme>& cellScheme, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase);

	void setup() final;

	inline const sem::ShipCellDataScheme& getShipCellData(size_t i) const { return *m_model.getDataModel().getIndexedShipCellData().get(m_dataIndices[i]); }

	inline const tdb::ZoneScheme* getZoneScheme() const { return m_zoneScheme; }

private:
	void computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j) final;

	void computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j) final;

	void computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j) final;

	const tdb::ZoneScheme* findPilotAssistanceZone(const tdb::IndexedZones& zones, const tdb::IndexedVertices& vertices) const;

	// Database references
	const SamsonExposureModel& m_model;				// Reference to model databases
	const tdb::TrafficDataBase& m_trafficDatabase;	// Reference to traffic database

	const tdb::ZoneScheme* m_zoneScheme;

};
} // namespace sem

