// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#define _USE_MATH_DEFINES 
#include <cmath>

#include "sem_export.h"
#include <sim/InteractionTypes.h>
#include <sem/DataModel.h>
#include <sem/Link.h>

namespace sem
{
class SEM_EXPORT LinkLink : public sim::LinkLink
{
	using base = sim::LinkLink;
public:
	LinkLink(const sim::Link& linkA, const sim::Link& linkB, const SamsonExposureModel& dataModel, const tdb::TrafficDataBase& trafficDatabase);

	void setup() final;

private:
	void computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j) final;

	void computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j) final;

	void computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j) final;

	void crossing(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j);
	
	void headOn(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j);

	void overtaking(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j);

	void findPilotAssistanceZone(const tdb::IndexedZones& zones, const tdb::IndexedVertices& vertices);

private:

	// References to data
	const Link& m_linkA;								// Reference to linkA
	const Link& m_linkB;								// Reference to linkB
	const SamsonExposureModel& m_model;					// Reference to the data, causation model
	const tdb::TrafficDataBase& m_trafficDatabase;		// Reference to the traffic database
	bool m_headOn;										// HeadOn model, otherwise crossing model

	// Data cache
	std::vector<std::vector<double>> m_pcollision;		// Chance that ship i collides with ship j
	const tdb::ZoneScheme* m_zoneScheme;				// Pilot assistance zone
};
} // namespace sem

