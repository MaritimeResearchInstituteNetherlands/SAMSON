// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sem_export.h"
#include <sim/InteractionTypes.h>
#include <sem/ExposureModel.h>
#include <sem/DataSchemes.h>
#include <tdb/TrafficDataBase.h>

namespace sem
{

// Link model, computing the imminent collisions of overtaking ships
class SEM_EXPORT Link : public sim::Link
{
	using base = sim::Link;
public:
	Link(const size_t& index, const tdb::Data<tdb::LinkScheme>& linkdb, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDataBase);

	void setup() final;

	inline const sem::ShipLinkDataScheme& getShipLinkData(size_t i) const { return *m_model.getDataModel().getIndexedShipLinkData().get(m_dataIndices[i]); }

	inline const tdb::ZoneScheme* getZoneScheme() const { return m_zoneScheme; }

private:
	void computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j) final;

	void computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j) final;

	void computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j) final;

	void createPolygon();

	void createRectangle();

	const tdb::ZoneScheme* findPilotAssistanceZone(const tdb::IndexedZones& zones, const tdb::IndexedVertices& vertices) const;

	// Database references
	const SamsonExposureModel& m_model;				// Reference to model databases
	const tdb::TrafficDataBase& m_trafficDataBase;	// Reference to traffic database

	// Link properties
	sml::Coordinate2D m_origin;						// Origin used for projection of link-only cases
	sml::Vector2d m_p0;								// Projected start point
	sml::Vector2d m_p1;								// Projected end point
	const tdb::ZoneScheme* m_zoneScheme;			// Pilot assistance zone
};
} // namespace sem

