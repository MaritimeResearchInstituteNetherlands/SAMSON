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
#include <sml/Polygon.h>

namespace sim
{

class SIM_EXPORT Cell : public ModelEntity, public TrafficEntity, public ShipShipEntity
{
public:
	Cell(const size_t& cellIndex, const tdb::Data<tdb::CellScheme>& cellScheme, const tdb::TrafficModel& trafficModel);

	inline std::string getDescription() { return std::string("cell ") + m_cellScheme->getUID(); }

	inline const std::string getUID() const { return m_cellScheme->getUID(); }

	inline const std::string getGridUID() const { return m_cellScheme->getUIDGrid(); }

	void computeExposures() final;

	inline const tdb::ShipCategoryScheme& getShipCategory(size_t i) const { return *m_trafficModel.getShipCategories().get(m_shipCategoryDataIndices[i]); }

	inline const tdb::ShipCellScheme& getShipCellScheme(size_t i) const { return *m_trafficModel.getShipCells().get(m_trafficIndex[i]); }

	inline const double& getArea() const { return m_area; }
	
	inline const sml::Coordinate2D& getOrigin() const { return m_origin; }

	inline const sml::Polygon& getPolygon() const { return m_polygon; }

protected:
	const tdb::Data<tdb::CellScheme> m_cellScheme;
	sml::Coordinate2D m_origin;
	sml::Polygon m_polygon;
	double m_area;

};
} // namespace sim
