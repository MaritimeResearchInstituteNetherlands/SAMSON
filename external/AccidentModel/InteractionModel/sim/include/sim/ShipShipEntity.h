// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sim/InteractionRoles.h>

namespace sim
{

struct ShipShipMeeting
{
	size_t index;														// Meeting index unique to the interactionType (e.g. Link, Cell)
	size_t i;															// Index to the first ship 
	size_t j;															// Index to the second ship or object
	double exposures = std::numeric_limits<double>::quiet_NaN();			// Candidate collision (ship-ship) or danger miles (ship-object)
	double struckChance_i = std::numeric_limits<double>::quiet_NaN();	// Chance that ship i is struck by ship j
	double struckChance_j = std::numeric_limits<double>::quiet_NaN();	// Chance that ship j is struck by ship i
	double events_i = std::numeric_limits<double>::quiet_NaN();			// Number of events that ship i is hit if no measure are taken
	double events_j = std::numeric_limits<double>::quiet_NaN();			// Number of events that ship j is hit if no measures are taken
	double collisions_i = std::numeric_limits<double>::quiet_NaN();		// Number of collisions that ship i is hit
	double collisions_j = std::numeric_limits<double>::quiet_NaN();		// Number of collisions that ship j is hit
};



class SIM_EXPORT ShipShipEntity : public IInteractionEntity
{
public:
	ShipShipEntity() : m_meetings() {}

	void computeEvents() final;

	void computeCollisions() final;

	inline const std::vector<ShipShipMeeting>& getMeetings() const { return m_meetings; }

protected:
	virtual void computeExposure(const ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j) = 0;

	virtual void computeEvent(const ShipShipMeeting& meeting, double& events_i, double& events_j) = 0;

	virtual void computeCollision(const ShipShipMeeting& meeting, double& collisions_i, double& collisions_j) = 0;

	std::vector<ShipShipMeeting> m_meetings;
};
} // namespace sim
