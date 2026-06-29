// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file Polygon.h
 * @brief This file contains the declaration of the Polygon class.
 */
#pragma once

#include <sml_export.h>

#include <sml/Vector.h>
#include <vector>
#include <tuple>

namespace sml {

class SML_EXPORT Polygon 
{
public: 
	Polygon();

	Polygon(const Polygon& p);
	
	inline void addVertex(const Vector2d& vertex) { m_vertices.push_back(vertex); }

	inline void insertVertex(const Vector2d& vertex, size_t pos) { m_vertices.insert(m_vertices.begin() + pos, vertex); }

	inline const size_t getVertexCount() const { return m_vertices.size(); }

	inline Vector2d& operator[](size_t i) { return m_vertices[i]; }

	inline const Vector2d& operator[](size_t i) const { return m_vertices[i]; }

	bool isInside(const Vector2d& p) const;

	std::tuple<sml::Vector2d, double> getCentroidAndArea() const;

	double getArea() const;

	std::vector<Polygon> getIntersectionWith(const Polygon& p) const;

	const std::tuple<double, double, double, double> getBoundingBox() const;

private:
	std::vector<Vector2d> m_vertices;

};



class PolygonClipper 
{
	enum class VertexType
	{
		Unvisited,
		Outside,
		Overlap,
		Inside,
		Enter,
		Exit
	};

	struct VertexState
	{
		VertexType type = VertexType::Unvisited;
		int tag = -1;
	};

	struct Part {
		int start;
		int length;
		bool isUsed = false;
	};

	using States = std::vector<VertexState>;
	using Parts = std::vector<Part>;
public:
	std::vector<Polygon> intersection(const Polygon& p0, const Polygon& p1);

private:
	States flagVertices(const Polygon& p, const Polygon& pOther);

	void insertIntersectionPoints();

	bool tagIntersections(States& state);

	Parts collectParts(const States& state);

	Part* findPart(States* states, Parts* parts, int tag);

	std::vector<Polygon> perturbedIntersection(const Polygon& p0, const Polygon& p1);

	Polygon constructPolygon(Part* startPart, const int startTag);

	Polygon m_p0;
	Polygon m_p1;
	States m_states0;
	States m_states1;
	Parts m_parts0;
	Parts m_parts1;
};

} // namespace sml
