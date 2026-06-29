// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/Cell.h>
#include <tdb/AreaModel.h>

namespace sim {
Cell::Cell(const size_t& cellIndex, const tdb::Data<tdb::CellScheme>& cellScheme, const tdb::TrafficModel& trafficModel)
	: ModelEntity(cellIndex)
	, TrafficEntity(trafficModel)
	, ShipShipEntity()
	, m_cellScheme(cellScheme)
	, m_area(0.0)
	, m_origin(0.0, 0.0)
{
}



void Cell::computeExposures()
{
	size_t numShips = getNumShips();
	m_meetings.reserve(numShips * numShips);
	for (size_t i = 0; i < numShips; i++) {
		for (size_t j = i; j < numShips; j++) {
			ShipShipMeeting meeting;
			meeting.index = i + j * numShips;
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
