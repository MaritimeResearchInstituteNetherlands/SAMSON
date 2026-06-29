// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/DataSchemes.h>

namespace sem
{
ShipLinkDataScheme::ShipLinkDataScheme()
	: m_uid("")
	, m_frequency(0)
	, m_speed(std::numeric_limits<double>::quiet_NaN())
	, m_mu(0.0)
	, m_sigma(0.0)
{
}



ShipCellDataScheme::ShipCellDataScheme()
	: m_uid("")
	, m_density(0.0)
	, m_speed(std::numeric_limits<double>::quiet_NaN())		// Take the default speed from ship data
	, m_direction(std::numeric_limits<double>::quiet_NaN())	// optional parameter, if not set it has all directions
{
}
} // namespace sem

