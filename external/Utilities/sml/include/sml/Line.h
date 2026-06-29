// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sml_export.h>
#include <sml/Vector.h>

namespace sml
{

class SML_EXPORT Line
{
public:
	Line();

	Line(sml::Vector2d start, sml::Vector2d end);

	Line(const Line& other);

	bool computeIntersection(const Line& other, sml::Vector2d& result) const;

	bool computeIntersection(const sml::Vector2d& point, const sml::Vector2d& dir, sml::Vector2d& result) const;

	bool projectPointOnLine(const sml::Vector2d& point, const sml::Vector2d& dir, sml::Vector2d& result, double eps = 0) const;

	bool projectPointOnInfiniteLine(const sml::Vector2d& point, const sml::Vector2d& dir, sml::Vector2d& result) const;

	bool isPointOnLine(const sml::Vector2d& point) const;

	bool isPointInsideLine(const sml::Vector2d& point) const;

	double distanceToPoint(const sml::Vector2d& point) const;

	inline const sml::Vector2d& getStart() const { return m_start; }

	inline const sml::Vector2d& getEnd() const { return m_end; }

	inline const sml::Vector2d& getDir() const { return m_dir; }

	inline const double getLength() const { return m_length; }
		
private:

	sml::Vector2d m_start;		// Start point of the line
	sml::Vector2d m_end;		// End point of the line
	sml::Vector2d m_dir;		// Direction of the line
	double m_length;			// Length of the line
};

} // namespace sml
