// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sml/AngleBounds.h>
#include <sml/Functions.h>
#include <sml/GeoUtils.h>
#include <sio/logger/api.h>
#include <cassert>

std::ostream& operator<<(std::ostream& s, const sml::AngleBounds& b) {
	s << "[" << b.getLowerBound() << ", " << b.getUpperBound() << "| " << b.getExtent() <<  "]";
	return s;
}

namespace sml {

AngleBounds::AngleBounds()
	: m_extent(0.0)
	, m_lowerBound(0.0)
	, m_upperBound(0.0)
{
}



AngleBounds::AngleBounds(double lowerBound, double upperBound, double extent)
	: m_lowerBound(sml::normalisedPositiveAngles(lowerBound))
	, m_upperBound(sml::normalisedPositiveAngles(upperBound))
	, m_extent(extent)
{
	assert(extent <= 2.0 * std::numbers::pi);
}



AngleBounds::AngleBounds(const AngleBounds& other)
	: m_lowerBound(other.m_lowerBound)
	, m_upperBound(other.m_upperBound)
	, m_extent(other.m_extent)
{
}



bool AngleBounds::isInRange(const double angle, bool inclusive) const
{
	double angle_ = sml::normalisedPositiveAngles(angle);

	if (inclusive) {
		if (m_upperBound < m_lowerBound) {
			bool inFirstPart = angle_ <= m_upperBound && angle_ >= 0.0;
			bool inSecondPart = angle_ >= m_lowerBound && angle_ <= 2.0 * std::numbers::pi;
			return inFirstPart || inSecondPart;
		}
		else {
			return angle_ <= m_upperBound && angle_ >= m_lowerBound;
		}
	}
	else {
		if (m_upperBound < m_lowerBound) {
			bool inFirstPart = angle_ < m_upperBound && angle_ >= 0.0;
			bool inSecondPart = angle_ > m_lowerBound && angle_ <= 2.0 * std::numbers::pi;
			return inFirstPart || inSecondPart;
		}
		else {
			return angle_ < m_upperBound && angle_ > m_lowerBound;
		}
	}
}



bool AngleBounds::hasOverlapWith(const AngleBounds& other) const
{
	if (isInRange(other.getLowerBound())) return true;
	if (isInRange(other.getUpperBound())) return true;
	if (other.isInRange(m_lowerBound)) return true;
	if (other.isInRange(m_upperBound)) return true;
	return false;
}



AngleBounds computeAngleBounds(const Vector2d& p0, const Vector2d& p1, const std::vector<sml::Vector2d>& vertices)
{
	AngleBounds bounds;
	Vector2d vertex = vertices[vertices.size() - 1];
	AngleBounds pBound = computePointBound(vertex, p0, p1);
	for (auto vertexNext : vertices) {
		// Compute xy position of point and generate the point bound
		AngleBounds pnextBound = computePointBound(vertexNext, p0, p1);
		AngleBounds lineBound = computeLineBound(pBound, pnextBound);

		// Merge the lineBound with the object bound
		bounds = mergeBounds(bounds, lineBound);

		// Prepare for next segment
		vertex = vertexNext;
		pBound = pnextBound;
	}
	return bounds;
}



AngleBounds computePointBound(const sml::Vector2d& p, const sml::Vector2d& l0, const sml::Vector2d& l1)
{
	AngleBounds bound;

	sml::Vector2d dp0 = l0 - p;
	sml::Vector2d dp1 = l1 - p;

	// Find the angles between p-p0 and p - p1 in change to the normalised positive representation [0, 2pi
	double anglea = std::atan2(-dp0[1], -dp0[0]);
	double angleb = std::atan2(-dp1[1], -dp1[0]);

	// Get the extent which should be within the range of [0,pi] and sort the angles from lower to upper bound
	double diff = angleb - anglea;
	double extent = std::abs(diff);
	if (extent > std::numbers::pi) {
		extent = 2.0 * std::numbers::pi - extent;
		if (diff > 0) {
			std::swap(anglea, angleb);
		}
	}
	else {
		if (diff < 0) {
			std::swap(anglea, angleb);
		}
	}

	// Compute the extent of the angle range and normalised angles
	bound.setExtent(extent);
	bound.setLowerBound(sml::normalisedPositiveAngles(anglea));
	bound.setUpperBound(sml::normalisedPositiveAngles(angleb));

	assert(std::abs(sml::normalisedPositiveAngles(bound.getLowerBound() + bound.getExtent()) - bound.getUpperBound()) < 1e-4);

	return bound;
}



// The bounds of a line can be created from the single point bounds, the smallest extent between these bounds is included
// Assuming the new extent can't be over 180 degrees, which is equivalent to saying that the line does not intersect with the
// link
AngleBounds computeLineBound(const AngleBounds& b0, const AngleBounds& b1)
{
	AngleBounds bound;
	double extent0 = std::abs(b0.getUpperBound() - b1.getLowerBound());
	double extent1 = std::abs(b1.getUpperBound() - b0.getLowerBound());
	if (extent0 > std::numbers::pi) {
		extent0 = 2.0 * std::numbers::pi - extent0;
	}
	if (extent1 > std::numbers::pi) {
		extent1 = 2.0 * std::numbers::pi - extent1;
	}

	// The largest difference is the new extent
	bound.setExtent(std::max(extent0, extent1));

	// Find the lowerbound
	bound.setLowerBound(b0.getLowerBound());
	bound.setUpperBound(sml::normalisedPositiveAngles(bound.getLowerBound() + bound.getExtent()));
	if (std::abs(bound.getUpperBound() - b1.getUpperBound()) > 1e-12) {
		bound.setLowerBound(b1.getLowerBound());
		bound.setUpperBound(sml::normalisedPositiveAngles(bound.getLowerBound() + bound.getExtent()));
	}

	return bound;
}



// Merges bounds, assuming that the bounds either connect or have overlap.
AngleBounds mergeBounds(const AngleBounds& b0, const AngleBounds& b1)
{
	AngleBounds bound;

	// Handle empty or full bounds
	if (b0.getExtent() == 0.0 || b1.getExtent() == 2.0 * std::numbers::pi) {
		return b1;
	}
	if (b1.getExtent() == 0.0 || b0.getExtent() == 2.0 * std::numbers::pi) {
		return b0;
	}

	// Find the lower bound
	size_t swapCount = 0;
	double lowerBound = b0.getLowerBound();
	if (b1.isInRange(lowerBound)) {
		lowerBound = b1.getLowerBound();
		swapCount++;
	}

	// Find the upper bound
	double upperBound = b0.getUpperBound();
	if (b1.isInRange(upperBound)) {
		upperBound = b1.getUpperBound();
		swapCount++;
	}

	if (swapCount == 2) {
		// It is possible that the circle is closed, test if an angle outside b0 is inside b1
		double testAngle = b0.getUpperBound() + 0.5 * (2.0 * std::numbers::pi - b0.getExtent());
		if (b1.isInRange(testAngle)) {
			bound.setLowerBound(0.0);
			bound.setUpperBound(2.0 * std::numbers::pi);
			bound.setExtent(2.0 * std::numbers::pi);
			return bound;
		}
	}

	// Compute the extent
	double extent = std::abs(upperBound - lowerBound);

	// Make sure the sign of the extent is correct
	double upperBound_ = sml::normalisedPositiveAngles(lowerBound + extent);
	if (std::abs(upperBound_ - upperBound) > 1e-12) {
		extent = 2.0 * std::numbers::pi - extent;
	}

	// Finalise bounds
	bound.setLowerBound(lowerBound);
	bound.setUpperBound(upperBound);
	bound.setExtent(extent);
	return bound;
}

} // namespace sml


