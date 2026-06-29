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
#include <sio/csv/csv_iarchive.h>
#include <sio/csv/csv_oarchive.h>

#include <sml/Vector.h>
#include <sml/Polygon.h>

namespace sim
{
class SIM_EXPORT Link : public ModelEntity, public TrafficEntity, public ShipShipEntity
{
public:
	Link(const size_t& linkIndex, const tdb::Data<tdb::LinkScheme>& linkScheme, const tdb::TrafficModel& trafficModel);

	inline std::string getDescription() { return std::string("Link") + m_linkScheme->getUID(); }

	void computeExposures() final;

	inline const tdb::ShipCategoryScheme& getShipCategory(size_t i) const { return *m_trafficModel.getShipCategories().get(m_shipCategoryDataIndices[i]); }

	inline const tdb::ShipLinkScheme& getShipLinkScheme(size_t i) const { return *m_trafficModel.getShipLinks().get(m_trafficIndex[i]); }

	inline void setWaypointStart(tdb::Data<tdb::WaypointScheme> wp) { m_wpStart = wp; }

	inline void setWaypointEnd(tdb::Data<tdb::WaypointScheme> wp) { m_wpEnd = wp; }

	inline const tdb::Data<tdb::WaypointScheme> getWaypointStart() const { return m_wpStart; }

	inline const tdb::Data<tdb::WaypointScheme> getWaypointEnd() const { return m_wpEnd; }

	inline const std::string getUID() const { return m_linkScheme->getUID(); }

	inline const tdb::Data<tdb::LinkScheme>& getLinkScheme() const { return m_linkScheme; }

	inline const sml::Vector2d& getDirection() { return m_direction; }

	inline const double& getLength() const { return m_length; }

	inline const double& getLeftBoundary() const { return m_leftBoundary; }

	inline const double& getRightBoundary() const { return m_rightBoundary; }

	inline const std::vector<sml::Coordinate2D>& getPolygon() const { return m_polygon; }

	inline const std::vector<sml::Coordinate2D>& getRectangle() const { return m_rectangle; }

protected:
	std::vector<sml::Coordinate2D> m_polygon;
	std::vector<sml::Coordinate2D> m_rectangle;
	const tdb::Data<tdb::LinkScheme> m_linkScheme;
	sml::Vector2d m_direction;			// Direction of the link in plane
	double m_length;
	double m_angle;
	double m_leftBoundary;
	double m_rightBoundary;
private:
	tdb::Data<tdb::WaypointScheme> m_wpStart;
	tdb::Data<tdb::WaypointScheme> m_wpEnd;
};
} // namespace sim
