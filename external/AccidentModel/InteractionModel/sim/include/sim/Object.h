// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sim/InteractionRoles.h>
#include <sml/Coordinate.h>

namespace sim
{
class SIM_EXPORT Object : public ModelEntity
{
public:
	Object(const size_t objectIndex, const tdb::Data<tdb::ObjectScheme>& objectScheme, const std::vector<sml::Coordinate2D>& coordinates);

	const std::string getUID() const { return m_objectScheme->getUID(); }

	const auto& getScheme() const { return m_objectScheme; }

	inline bool isClosed() const { return m_isClosed; }

	inline double getArea() const { return m_area; }
protected:
	bool m_isClosed;
	double m_area;
	const tdb::Data<tdb::ObjectScheme> m_objectScheme;
};
} // namespace sim
