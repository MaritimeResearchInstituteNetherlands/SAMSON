// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sim/InteractionRoles.h>
#include <sml/Vector.h>

namespace sim
{
struct ShipObjectMeeting
{
	size_t index;							// Meeting index unique to the interactionType
	size_t i;								// Local link ship index
	size_t j;								// Object index
	double driftExposure = 0.0;				// Summed danger distances for drift (per second rate?) // TODO
	double driftEvents = 0.0;				// Summed drift events per second
	double driftCollisions = 0.0;			// Summed drift collisions per second
	double ramExposure = 0.0;				// Summed danger distances for ram (// TODO)
	double ramEvents = 0.0;					// Summed ram events per second
	double ramCollisions = 0.0;				// Summed ram collisions per second
};



class SIM_EXPORT ShipObjectEntity : public IInteractionEntity
{
public:
	ShipObjectEntity();

	void setup();

	inline void setNumShips(size_t numShips) { m_numShips = numShips; }

	inline void setNumObjects(size_t numObjects) { m_numObjects = numObjects; }

	void computeExposures() final;

	void computeEvents() final;

	void computeCollisions() final;

	inline const std::vector<ShipObjectMeeting>& getMeetings() const { return m_meetings; }

protected:
	void generateMeetings(size_t numShips, size_t numObjects);

	virtual void prepareExposureComputation() = 0;

	virtual void prepareEventComputation() = 0;

	virtual void prepareCollisionComputation() = 0;

	virtual void computeExposure(const sim::ShipObjectMeeting& meeting, double& driftExposure, double& ramExposure) = 0;

	virtual void computeEvents(const sim::ShipObjectMeeting& meeting, double& driftEvents, double& ramEvents) = 0;

	virtual void computeCollisions(const sim::ShipObjectMeeting& meeting, double& driftCollisions, double& ramCollisions) = 0;

	// Output data
	size_t m_numShips;
	size_t m_numObjects;
	std::vector<ShipObjectMeeting> m_meetings;
};

}
