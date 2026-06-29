// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sim/InteractionRoles.h>
#include <sim/ShipShipEntity.h>
#include <sim/Link.h>
#include <sim/Cell.h>

namespace sim
{
class SIM_EXPORT LinkCell : public ShipShipEntity
{
public:
	LinkCell(const Link& link, const Cell& cell);

	inline std::string getDescription() { return std::string("linkcell with link ") + m_link.getUID() + " and cell " + m_cell.getUID(); }

	void computeExposures() final;

	inline const Link& getLink() const { return m_link; }

	inline const Cell& getCell() const { return m_cell; }

protected:
	const Link& m_link;
	const Cell& m_cell;
};
} // namespace sim
