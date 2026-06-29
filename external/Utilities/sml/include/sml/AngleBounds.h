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
#include <sml/Coordinate.h>
#include <sml/Functions.h>
#include <vector>
#include <ostream>

namespace sml {

	class SML_EXPORT AngleBounds
	{
	public:
		AngleBounds();

		AngleBounds(double lowerBound, double upperBound, double extent);

		AngleBounds(const AngleBounds& other);

		inline const double getLowerBound() const { return m_lowerBound; }

		inline void setLowerBound(const double b) { m_lowerBound = b; }

		inline const double getUpperBound() const { return m_upperBound; }

		inline void setUpperBound(const double b) { m_upperBound = b; }

		inline const double getExtent() const { return m_extent; }

		inline void setExtent(const double e) { m_extent = e; }

		bool isInRange(const double angle, bool inclusive = true) const;

		bool hasOverlapWith(const AngleBounds& other) const;

		double getMidAngle() const { return sml::normalisedPositiveAngles(m_lowerBound + 0.5 * m_extent); }

	private:
		double m_lowerBound;	// Angle in [0, 2pi)
		double m_upperBound;	// Angle in [0, 2pi)
		double m_extent;		// the extent of the bound, defined as normalisedAngle(lowerBound + extent) = upperBound
	};

	SML_EXPORT AngleBounds computeAngleBounds(const Vector2d& p0, const Vector2d& p1, const std::vector<sml::Vector2d>& coordinates);

	SML_EXPORT AngleBounds computePointBound(const Vector2d& p, const Vector2d& l0, const Vector2d& l1);

	AngleBounds computeLineBound(const AngleBounds& b0, const AngleBounds& b1);

	SML_EXPORT AngleBounds mergeBounds(const AngleBounds& b0, const AngleBounds& b1);
} // namespace sml

SML_EXPORT std::ostream& operator<<(std::ostream& s, const sml::AngleBounds& b);
