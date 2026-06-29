// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/ShipShipEntity.h>

namespace sim
{

void ShipShipEntity::computeEvents()
{
	for_each(m_meetings.begin(), m_meetings.end(), [&](ShipShipMeeting& meeting)
		{
			computeEvent(meeting, meeting.events_i, meeting.events_j);
		}
	);
}



void ShipShipEntity::computeCollisions()
{
	for_each(m_meetings.begin(), m_meetings.end(), [&](ShipShipMeeting& meeting)
		{
			computeCollision(meeting, meeting.collisions_i, meeting.collisions_j);
		}
	);	
}

} // namespace sim
