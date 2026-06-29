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
#include <sim/Link.h>
#include <tdb/TrafficModel.h>

namespace sim
{
class SIM_EXPORT LinkLink : public ShipShipEntity
{
public:
	LinkLink(const Link& linkA, const Link& linkB, const tdb::IndexedShipCategories& shipCategories);

	inline std::string getDescription() { return std::string("linklink with ") + m_linkA.getUID() + " and " + m_linkB.getUID(); }

	void computeExposures() final;

	inline void setAngle(const double& angle) { m_angle = angle; }

	inline double getAngle() const { return m_angle; }

	inline const Link& getLinkA() const { return m_linkA; }

	inline const Link& getLinkB() const { return m_linkB; }

	inline const tdb::IndexedShipCategories& getShipCategories() const{ return m_shipCategories; }

private:
	double m_angle;
	const Link& m_linkA;
	const Link& m_linkB;
	const tdb::IndexedShipCategories& m_shipCategories;
};
} // namespace sim
