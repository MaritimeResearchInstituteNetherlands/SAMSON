// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <tdb/TrafficSchemes.h>

namespace sem
{

class CausationFactorsScheme : public tdb::IIdentifiable
{
public:
	using UidType = std::string;

	CausationFactorsScheme();

	inline const std::string getUID() const override { return m_shipUid; }

	inline const tdb::ShipCategoryScheme::UidType getShipUID() const { return m_shipUid; }

	inline const double getShipShipFactor() const { return m_shipShipFactor; }

	inline const double getDriftFactor() const { return m_driftFactor; }

	inline const double getRamFactor() const { return m_ramFactor; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("ship_uid", m_shipUid);
		ar& boost::serialization::make_nvp("ship_ship_factor", m_shipShipFactor);
		ar& boost::serialization::make_nvp("drift_factor", m_driftFactor);
		ar& boost::serialization::make_nvp("ram_factor", m_ramFactor);
	}

private:
	tdb::ShipCategoryScheme::UidType m_shipUid;
	double m_shipShipFactor;
	double m_driftFactor;
	double m_ramFactor;
};
} // namespace sem

