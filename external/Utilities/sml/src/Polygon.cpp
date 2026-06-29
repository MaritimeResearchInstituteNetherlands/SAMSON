// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sml/Polygon.h>
#include <sml/Line.h>
#include <sio/logger/api.h>

namespace sml {

Polygon::Polygon()
{
}



Polygon::Polygon(const Polygon& p)
	: m_vertices(p.m_vertices)
{
}



// Shoots a horizontal array through all polygon edges from the evaluation point p
// Points on the boundary may or may not be included
bool Polygon::isInside(const Vector2d& p) const
{
	size_t numVertices = m_vertices.size();
	if (numVertices < 3) {
		LogError("The polygon does not have enough vertices.");
	}

	bool isInside = false;
	size_t nextIndex = numVertices - 1;
	for (size_t i = 0; i < numVertices; i++)
	{
		const Vector2d& nextVertex = m_vertices[nextIndex];
		const Vector2d& currentVertex = m_vertices[i];

		// If the current and next vertex lie above or below the current point then there is no intersection
		bool hasCrossing = (nextVertex[1] > p[1]) != (currentVertex[1] > p[1]);
		if (hasCrossing) {
			const Vector2d dir = currentVertex - nextVertex;
			double t = (p[1] - nextVertex[1]) / dir[1];	// Solution to the equation of the intersection point
			double intersectionPointX = nextVertex[0] + dir[0] * t;
			if (p[0] < intersectionPointX) {
				// Intersection happens to the right-side of the plane, traversing a boundary then.
				isInside = !isInside;
			}
		}

		// Prepare for next round
		nextIndex = i;
	}
	return isInside;
}



std::tuple<sml::Vector2d, double> Polygon::getCentroidAndArea() const
{
	sml::Vector2d centroid(0.0, 0.0);
	double area = 0.0;
	size_t i0 = m_vertices.size() - 1;
	for (size_t i1 = 0; i1 < m_vertices.size(); i1++) {
		const sml::Vector2d& p0 = m_vertices[i0];
		const sml::Vector2d& p1 = m_vertices[i1];
		double cross = p0[0] * p1[1] - p1[0] * p0[1];

		// Update values
		area += cross;
		centroid[0] += (p0[0] + p1[0]) * cross;
		centroid[1] += (p0[1] + p1[1]) * cross;

		// Next vertex
		i0 = i1;
	}

	// Finalise
	area *= 0.5;
	centroid = centroid/ (6 * area);
	return std::make_tuple(centroid, area);
}



double Polygon::getArea() const
{
	// Chop in triangles and compute their area
	double sum = 0.0;
	const Vector2d& p0 = m_vertices[m_vertices.size() - 1];
	for (size_t i = 0; i < m_vertices.size() - 2; i++) {
		const Vector2d& p1 = m_vertices[i];
		const Vector2d& p2 = m_vertices[i + 1];

		Vector2d p1p0 = p1 - p0;
		Vector2d p2p0 = p2 - p0;
		sum += p1p0[0] * p2p0[1] - p1p0[1] * p2p0[0];
	}
	return 0.5 * std::abs(sum);
}



std::vector<Polygon> Polygon::getIntersectionWith(const Polygon& p) const
{
	return PolygonClipper().intersection(*this, p);
}



const std::tuple<double, double, double, double> Polygon::getBoundingBox() const
{
	double xMin = DBL_MAX;
	double xMax = -DBL_MAX;
	double yMin = DBL_MAX;
	double yMax = -DBL_MAX;

	for (size_t i = 0; i < m_vertices.size(); i++) {
		xMin = xMin > m_vertices[i][0] ? m_vertices[i][0] : xMin;
		yMin = yMin > m_vertices[i][1] ? m_vertices[i][1] : yMin;

		xMax = xMax < m_vertices[i][0] ? m_vertices[i][0] : xMax;
		yMax = yMax < m_vertices[i][1] ? m_vertices[i][1] : yMax;
	}
	return std::make_tuple(xMin, xMax, yMin, yMax);
}



std::vector<Polygon> PolygonClipper::intersection(const Polygon& polygon0, const Polygon& polygon1)
{
	m_p0 = polygon0;
	m_p1 = polygon1;
	m_states0 = flagVertices(m_p0, m_p1);
	m_states1 = flagVertices(m_p1, m_p0);

	insertIntersectionPoints();

	// Check if one polygon lies in the other otherwise there is simply no overlap
	std::vector<Polygon> intersection;
	if (polygon0.getVertexCount() == m_p0.getVertexCount()) {
		if (m_states0[0].type == VertexType::Inside) {
			intersection.push_back(m_p0);
		}
		else if (m_states1[0].type == VertexType::Inside) {
			intersection.push_back(m_p1);
		} 
		return intersection;
	}

	// Tag the intersection points and collect intersection parts
	if (!tagIntersections(m_states0) || !tagIntersections(m_states1)) {
		// Try a perturbed polygon
		return perturbedIntersection(polygon0, polygon1);
	}
	m_parts0 = collectParts(m_states0);
	m_parts1 = collectParts(m_states1);
	if (m_parts0.size() != m_parts1.size()) {
		LogError("Exception found during computation of intersection of two polygons.");
	}

	// Glue all parts together
	for (auto& part : m_parts0) {
		if (!part.isUsed) {
			int startTag = m_states0[part.start].tag;
			intersection.push_back(constructPolygon(&part, startTag));
		}
	}

	return intersection;
}



std::vector<Polygon> PolygonClipper::perturbedIntersection(const Polygon& p0, const Polygon& p1)
{
	Polygon p0Perturbed(p0);
	for (size_t i = 0; i < p0.getVertexCount(); i++) {
		p0Perturbed[i][0] += 1e-3 * rand() / double(RAND_MAX);
		p0Perturbed[i][1] += 1e-3 * rand() / double(RAND_MAX);
	}
	return intersection(p0Perturbed, p1);
}



PolygonClipper::States PolygonClipper::flagVertices(const Polygon& p, const Polygon& pOther)
{
	States state(p.getVertexCount());

	// Check if points lie inside or outside
	for (size_t i = 0; i < state.size(); i++) {
		state[i].type = pOther.isInside(p[i]) ? VertexType::Inside : VertexType::Outside;
	}

	return state;
}



void PolygonClipper::insertIntersectionPoints()
{
	int tag = 0;
	for (int i = 0; i < m_p0.getVertexCount(); i++) { // Vertex count may increase during the loop
		int inext = (i + 1) % m_p0.getVertexCount();
		sml::Line edge0(m_p0[i], m_p0[inext]);
		for (int j = 0; j < m_p1.getVertexCount(); j++) {
			int jnext = (j + 1) % m_p1.getVertexCount();
			sml::Line edge1(m_p1[j], m_p1[jnext]);

			// Evaluate the edges, can only be one intersection between the edges
			sml::Vector2d result;
			if (edge0.computeIntersection(edge1, result)) {
				sml::Vector2d vertex = edge0.getStart() + edge0.getDir() * result[0];
				//std::cout << "Intersection found at " << vertex << std::endl;
				// Intersection is always added behind index i, so it can be guaranteed that i=0 is never an intersection
				m_p0.insertVertex(vertex, i + 1);
				m_p1.insertVertex(vertex, j + 1);
				m_states0.insert(m_states0.begin() + i + 1, VertexState(VertexType::Unvisited, tag));
				m_states1.insert(m_states1.begin() + j + 1, VertexState(VertexType::Unvisited, tag));
				tag++;

				// Update current edge with new intersection point
				edge0 = sml::Line(edge0.getStart(), m_p0[i + 1]);
			}
		}
	}
}



bool PolygonClipper::tagIntersections(States& states)
{
	// First vertex is never an intersection vertex
	bool isInsidePrevious = states[0].type == VertexType::Inside ? true : false;
	for (int i = 1; i < states.size(); i++) {
		const auto nextType = states[(i + 1) % states.size()].type;
		if (states[i].type == VertexType::Unvisited) {
			if (isInsidePrevious) {
				if (nextType == VertexType::Unvisited || nextType == VertexType::Outside) {
					states[i].type = VertexType::Exit;
					isInsidePrevious = false;
				}
				else {
					return false; // There is an intersection missing
				}
			} else {
				if (nextType == VertexType::Unvisited || nextType == VertexType::Inside) {
					states[i].type = VertexType::Enter;
					isInsidePrevious = true;
				} else {
					return false; // There is an intersection missing
				}
			}
		}
		else if ((isInsidePrevious && nextType == VertexType::Outside) || (!isInsidePrevious && nextType == VertexType::Inside)) {
			return false; // There is an intersection missing
		}
	}
	return true;
}



PolygonClipper::Parts PolygonClipper::collectParts(const States& state)
{
	Parts parts;
	for (size_t i = 0; i < state.size(); i++) {
		if (state[i].type == VertexType::Enter) {
			int iExit = (i + 1) % state.size();
			int len = 2;
			while (state[iExit].type != VertexType::Exit) {
				iExit = (iExit + 1) % state.size();
				len++;
				if (len > 2.0 * state.size()) {
					LogError("An error occurent during the clipping of two polygons.");
					break;
				}
			}
			parts.push_back(Part(i, len));
		}
	}
	return parts;
}



PolygonClipper::Part* PolygonClipper::findPart(States* states, Parts* parts, int tag)
{
	Parts& pref = *parts;
	States& sref = *states;
	for (auto& part : pref) {
		if (sref[part.start].tag == tag) {
			return &part;
		}
	}
	LogError("part with tag " << tag << " expected but not found.");
	return nullptr;
}



Polygon PolygonClipper::constructPolygon(Part* startPart, const int startTag)
{
	Polygon pnew;
	// Initialise swapping routine
	States* currentStates = &m_states0;
	States* otherStates = &m_states1;
	Parts* currentParts = &m_parts0;
	Parts* otherParts = &m_parts1;
	Part* currentPart = startPart;
	const Polygon* currentPolygon = &m_p0;
	const Polygon* otherPolygon = &m_p1;
	int nextTag = -1;
	while (nextTag != startTag) {
		// Add part to the intersection
		currentPart->isUsed = true;
		for (int j = 0; j < currentPart->length - 1; j++) { // last vertex is start of second part
			int index = (currentPart->start + j) % currentStates->size();
			pnew.addVertex((*currentPolygon)[index]);
		}
		size_t endIndex = (currentPart->start + currentPart->length - 1) % currentStates->size();
		nextTag = (*currentStates)[endIndex].tag;
		currentPart = findPart(otherStates, otherParts, nextTag);// todo perhaps use pointer as input
		std::swap(currentStates, otherStates);
		std::swap(currentParts, otherParts);
		std::swap(currentPolygon, otherPolygon);
	}
	return pnew;
}

} // namespace sml

    

 
