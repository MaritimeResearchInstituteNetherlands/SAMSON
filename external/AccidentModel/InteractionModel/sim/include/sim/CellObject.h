// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sim/InteractionRoles.h>
#include <sim/ShipObjectEntity.h>
#include <sim/Cell.h>
#include <sim/Object.h>

namespace sim
{
class SIM_EXPORT CellObject : public ShipObjectEntity
{
public:
	CellObject(const Cell& cell);

	inline std::string getDescription() { return std::string("cellObject with cell ") + m_cell.getUID(); }

	inline void addObject(const sim::Object* object) { m_objects.push_back(object); }

	inline const std::vector<const Object*> getObjects() const { return m_objects; }

	inline const Cell& getCell() const { return m_cell; }

	inline const sml::Coordinate2D& getOrigin() const { return m_cell.getOrigin(); }
private:
	const Cell& m_cell;
	std::vector<const Object*> m_objects;
};
} // namespace sim
