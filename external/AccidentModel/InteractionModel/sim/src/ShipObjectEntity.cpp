// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/ShipObjectEntity.h>

namespace sim
{

ShipObjectEntity::ShipObjectEntity()
	: m_numShips(0)
	, m_numObjects(0)
{
}



void ShipObjectEntity::setup()
{
}



void ShipObjectEntity::computeExposures()
{
	if (m_numShips > 0) {
		prepareExposureComputation();

		m_meetings.reserve(m_numShips * m_numObjects);

		for (size_t i = 0; i < m_numShips; i++) {
			for (size_t j = 0; j < m_numObjects; j++) {
				ShipObjectMeeting meeting;
				meeting.index = i + j * m_numShips; // TODO is not required
				meeting.i = i;
				meeting.j = j;
				computeExposure(meeting, meeting.driftExposure, meeting.ramExposure);
				if (meeting.driftExposure != 0.0 || meeting.ramExposure != 0.0) {
					m_meetings.emplace_back(meeting);
				}
			}
		}

		m_meetings.shrink_to_fit();
	}
}



void ShipObjectEntity::computeEvents()
{
	if (m_meetings.size() > 0) {
		prepareEventComputation();

		for_each(m_meetings.begin(), m_meetings.end(), [&](ShipObjectMeeting& meeting)
			{
				computeEvents(meeting, meeting.driftEvents, meeting.ramEvents);
			}
		);
	}
}



void ShipObjectEntity::computeCollisions()
{
	if (m_meetings.size() > 0) {
		prepareCollisionComputation();
		for_each(m_meetings.begin(), m_meetings.end(), [&](ShipObjectMeeting& meeting)
			{
				computeCollisions(meeting, meeting.driftCollisions, meeting.ramCollisions);
			}
		);
	}
}
} // namespace sim
