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
#include <sim/Link.h>
#include <sim/Object.h>

namespace sim
{
class SIM_EXPORT LinkObject : public ShipObjectEntity
{
public:
	LinkObject(const Link& link);

	inline std::string getDescription() { return std::string("linkobject with link ") + m_link.getUID(); }

	inline void addObject(const sim::Object* object) { m_objects.push_back(object); }

	inline const Link& getLink() const { return m_link; }

	inline const std::vector<const Object*> getObjects() const { return m_objects; }

	inline const Object* getObject(size_t i) const { return i < m_objects.size() ? m_objects[i] : nullptr; }
	
	inline const sml::Coordinate2D& getOrigin() const { return m_origin; }

	inline const sml::Vector2d& getP0() const { return m_p0; }

	inline const sml::Vector2d& getP1() const { return m_p1; }

private:
	sml::Coordinate2D computeOrigin(const sml::Coordinate2D& c0, const sml::Coordinate2D& c1);

	// Data references
	const Link& m_link;

	// List of interacting objects
	std::vector<const Object*> m_objects;

	// Transformation to 2D plane
	const sml::Coordinate2D m_origin;
	const sml::Vector2d m_p0;
	const sml::Vector2d m_p1;

};
} // namespace sim
