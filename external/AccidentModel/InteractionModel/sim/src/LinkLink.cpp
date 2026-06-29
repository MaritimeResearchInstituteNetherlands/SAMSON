// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/LinkLink.h>

namespace sim
{

LinkLink::LinkLink(const Link& linkA, const Link& linkB, const tdb::IndexedShipCategories& shipCategories)
	: m_linkA(linkA)
	, m_linkB(linkB)
	, m_shipCategories(shipCategories)
{
}



void LinkLink::computeExposures()
{
	size_t numShipsA = m_linkA.getNumShips();
	size_t numShipsB = m_linkB.getNumShips();
	m_meetings.reserve(numShipsA * numShipsB);
	for (size_t i = 0; i < numShipsA; i++) {
		for (size_t j = 0; j < numShipsB; j++) {
			ShipShipMeeting meeting;
			meeting.index = i + j * numShipsA;
			meeting.i = i;
			meeting.j = j;
			computeExposure(meeting, meeting.exposures, meeting.struckChance_i, meeting.struckChance_j);
			if (meeting.exposures != 0.0) {
				m_meetings.emplace_back(meeting);
			}
		}
	}
	m_meetings.shrink_to_fit();
}

} // namespace sim
