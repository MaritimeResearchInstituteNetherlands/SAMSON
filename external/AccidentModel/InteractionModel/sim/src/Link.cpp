// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/Link.h>

namespace sim
{
Link::Link(const size_t& linkIndex, const tdb::Data<tdb::LinkScheme>& linkScheme, const tdb::TrafficModel& trafficModel)
	: ModelEntity(linkIndex)
	, TrafficEntity(trafficModel)
	, ShipShipEntity()
	, m_linkScheme(linkScheme)
	, m_direction({ 0.0, 0.0 })
	, m_angle(-1)
	, m_length(-1)
	, m_leftBoundary(-1)
	, m_rightBoundary(-1)
{
}



void Link::computeExposures()
{
	size_t numShips = getNumShips();
	m_meetings.reserve(numShips * numShips);
	for (size_t i = 0; i < numShips; i++) {
		for (size_t j = 0; j < numShips; j++) {
			if (i != j) {
				ShipShipMeeting meeting;
				meeting.index = i + j * numShips; // Is not required
				meeting.i = i;
				meeting.j = j;
				computeExposure(meeting, meeting.exposures, meeting.struckChance_i, meeting.struckChance_j);
				if (meeting.exposures != 0.0) {
					m_meetings.emplace_back(meeting);
				}
			}
		}
	}
	m_meetings.shrink_to_fit();
}
} // namespace sim
