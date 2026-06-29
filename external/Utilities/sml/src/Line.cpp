// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sml/Line.h>

namespace sml 
{

Line::Line()
	: m_start(0.0, 0.0)
	, m_end(0.0, 0.0)
	, m_dir(1.0, 0.0)
	, m_length(0.0)
{
}



Line::Line(sml::Vector2d start, sml::Vector2d end)
	: m_start(start)
	, m_end(end)
	, m_dir(1.0, 0.0)
	, m_length(0.0)
{
	m_length = (m_end - m_start).norm();
	if (m_length != 0.0) {
		m_dir = (m_end - m_start) / m_length;
	}
}



Line::Line(const Line& other)
	: m_start(other.m_start)
	, m_end(other.m_end)
	, m_dir(other.m_dir)
	, m_length(other.m_length)
{
}



// The intersection is defined by line.start + line.dir * t0 = point + direction * t1
// where t0 \in (0,length) and t1 >= 0
// If these constraints are violated there is no intersection
// The system can be rewritten to matrix vector equation
// | line.dir[0] -direction[0] | | tline | =		| point[0] - line.start[0] |
// | line.dir[1] -direction[1] |	| tpoint| =		| point[1] - line.start[1] |
// returns in tline tpoint space
bool Line::computeIntersection(const sml::Vector2d& point, const sml::Vector2d& dir, sml::Vector2d& result) const
{
	// Check for a solution
	double detInv = dir[0] * m_dir[1] - m_dir[0] * dir[1];
	if (detInv == 0.0) {
		return false;
	}

	// Solve the system
	double det = 1.0 / detInv;
	double b0 = point[0] - m_start[0];
	double b1 = point[1] - m_start[1];
	result[0] = det * (dir[0] * b1 - dir[1] * b0);			// tline
	result[1] = det * (m_dir[0] * b1 - m_dir[1] * b0);		// tpoint

	return true;
}



bool Line::computeIntersection(const Line& other, sml::Vector2d& result) const
{
	computeIntersection(other.getStart(), other.getDir(), result);

	// Check result bounds
	constexpr double eps = 1e-8;
	if (result[0] > eps && result[0] < m_length - eps && result[1] > eps && result[1] < other.getLength() - eps) {
		return true;
	}
	return false;
}



// Projects a point on a line
bool Line::projectPointOnLine(const sml::Vector2d& point, const sml::Vector2d& dir, sml::Vector2d& result, double eps) const
{
	if (!computeIntersection(point, dir, result)) {
		return false;
	}

	// Validate constraints and compute point
	double tline = result[0];
	double tpoint = result[1];
	result[0] = m_start[0] + m_dir[0] * tline;
	result[1] = m_start[1] + m_dir[1] * tline;
	double tlineScaled = tline / m_length;
	if (tlineScaled > -eps && tlineScaled < (1.0 + eps) && tpoint >= -eps) { // Include a small area around the start and end points
		return true;
	}
	else {
		return false;
	}
}






bool Line::projectPointOnInfiniteLine(const sml::Vector2d& point, const sml::Vector2d& dir, sml::Vector2d& result) const
{
	if (!computeIntersection(point, dir, result)) {
		return false;
	}

	// Intersection point
	double tline = result[0];
	double tpoint = result[1];
	result[0] = m_start[0] + m_dir[0] * tline;
	result[1] = m_start[1] + m_dir[1] * tline;

	// True if the intersection point is in the positive dir-direction
	if (tpoint >= 0.0) {
		return true;
	}
	else {
		return false;
	}
}



bool Line::isPointOnLine(const sml::Vector2d& point) const
{
	// Ensure the point is on the line
	sml::Vector2d dirp = point - m_start;
	double dirpx = point[0] - m_start[0];
	double dirpy = point[1] - m_start[1];
	double invNorm = 1.0 / std::sqrt(dirpx * dirpx + dirpy * dirpy);
	double cross = (dirp[0] * m_dir[1] - dirp[1] * m_dir[0]) * invNorm;
	if (std::abs(cross) > 1e-12) {
		return false;
	}

	// Check if it is between bounds by verifying the point lies in the line bound for a representable direction (dir-y could be zero..)
	if (m_dir[0] != 0.0) {
		if (m_dir[0] > 0.0) {
			return point[0] >= m_start[0] && point[0] <= m_end[0];
		}
		else {
			return point[0] <= m_start[0] && point[0] >= m_end[0];
		}
	}
	else {
		if (m_dir[1] > 0.0) {
			return point[1] >= m_start[1] && point[1] <= m_end[1];
		}
		else {
			return point[1] <= m_start[1] && point[1] >= m_end[1];
		}
	}
}



bool Line::isPointInsideLine(const sml::Vector2d& point) const
{
	double cross = (point[0] - m_start[0]) * m_dir[1] - (point[1] - m_start[1]) * m_dir[0];
	if (std::fabs(cross) > 1e-12) {
		return false;
	}

	// Check if it is between bounds by verifying the point lies in the line bound for a representable direction (dir-y could be zero..)
	if (m_dir[0] != 0.0) {
		if (m_dir[0] > 0.0) {
			return point[0] > m_start[0] && point[0] < m_end[0];
		}
		else {
			return point[0] < m_start[0] && point[0] > m_end[0];
		}
	}
	else {
		if (m_dir[1] > 0.0) {
			return point[1] > m_start[1] && point[1] < m_end[1];
		}
		else {
			return point[1] < m_start[1] && point[1] > m_end[1];
		}
	}
}



double Line::distanceToPoint(const sml::Vector2d& point) const
{
	// Todo generalise this system solving
	// Orthogonal to line
	sml::Vector2d dir(-m_dir[1], m_dir[0]);

	double detInv = dir[0] * m_dir[1] - m_dir[0] * dir[1];
	if (detInv == 0.0) {
		return false;
	}

	// Solve the system
	double det = 1.0 / detInv;
	double b0 = point[0] - m_start[0];
	double b1 = point[1] - m_start[1];
	double tline = det * (dir[0] * b1 - dir[1] * b0);
	double tpoint = det * (m_dir[0] * b1 - m_dir[1] * b0);
	
	if (tline < 0) {
		// Not orthogonal, then the shortest distance to start
		return (point - m_start).norm();
	}
	if (tline > m_length) {
		return (point - m_end).norm();
	}
	else {
		// Orthogonal
		return std::abs(tpoint);
	}
}
} // namespace sml
