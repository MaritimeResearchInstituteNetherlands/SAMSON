// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/CausationSchemes.h>

namespace sem
{
	CausationFactorsScheme::CausationFactorsScheme()
	: m_shipUid("")
	, m_shipShipFactor(0.0)
	, m_driftFactor(0.0)
	, m_ramFactor(0.0)
{
}
} // namespace sem

