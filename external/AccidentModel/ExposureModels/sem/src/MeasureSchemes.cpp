// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/MeasureSchemes.h>

namespace sem {

PilotAssistanceShipScheme::PilotAssistanceShipScheme()
	: m_shipUid("")
	, m_probability(0.0)
{
}

PilotAssistanceZoneScheme::PilotAssistanceZoneScheme()
	: m_zoneUid("")
	, m_probability(0.0)
{
}

} // namespace sem
