// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/csv/OutputSchemes.h>
#include <limits>

namespace sim
{
namespace csv
{

BaseShipShipScheme::BaseShipShipScheme()
	: m_meeting()
	, m_trafficAUID("")
	, m_trafficBUID("")
{
}



BaseShipObjectScheme::BaseShipObjectScheme()
	: m_meetingIndex(-1)
	, m_objectUID("")
	, m_trafficUID("")
	, m_driftExposure(std::numeric_limits<double>::quiet_NaN())
	, m_ramExposure(std::numeric_limits<double>::quiet_NaN())
	, m_driftEvents(std::numeric_limits<double>::quiet_NaN())
	, m_ramEvents(std::numeric_limits<double>::quiet_NaN())
	, m_driftCollisions(std::numeric_limits<double>::quiet_NaN())
	, m_ramCollisions(std::numeric_limits<double>::quiet_NaN())
{
}



LinkScheme::LinkScheme()
	: base()
	, m_linkUID("")
{
}



LinkLinkScheme::LinkLinkScheme()
	: base()
	, m_linkAUID("")
	, m_linkBUID("")
{
}



LinkObjectScheme::LinkObjectScheme()
	: base()
	, m_linkUID("")
{
}



CellScheme::CellScheme()
	: base()
	, m_cellUID("")
	, m_gridUID("")
{
}



CellObjectScheme::CellObjectScheme()
	: base()
	, m_cellUID("")
{
}



LinkCellScheme::LinkCellScheme()
	: base()
	, m_linkUID("")
	, m_cellUID("")
	, m_gridUID("")
{
}



} // namespace csv
} // namespace sim
