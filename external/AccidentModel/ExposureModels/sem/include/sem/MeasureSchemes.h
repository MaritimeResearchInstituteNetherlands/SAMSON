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

	class PilotAssistanceShipScheme : public tdb::IIdentifiable
	{
	public:
		using UidType = std::string;

		PilotAssistanceShipScheme();

		inline const std::string getUID() const override { return m_shipUid; }

		inline const tdb::ShipCategoryScheme::UidType getShipUID() const { return m_shipUid; }

		inline const double getProbability() const { return m_probability; }

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("ship_uid", m_shipUid);
			ar& boost::serialization::make_nvp("probability", m_probability);
		}

	private:
		tdb::ShipCategoryScheme::UidType m_shipUid;
		double m_probability;
	};


	class PilotAssistanceZoneScheme : public tdb::IIdentifiable
	{
	public:
		using UidType = std::string;

		PilotAssistanceZoneScheme();

		inline const std::string getUID() const override { return m_zoneUid; }

		inline const tdb::ShipCategoryScheme::UidType getZoneUID() const { return m_zoneUid; }

		inline const double getProbability() const { return m_probability; }

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("zone_uid", m_zoneUid);
			ar& boost::serialization::make_nvp("probability", m_probability);
		}

	private:
		tdb::ZoneScheme::UidType m_zoneUid;
		double m_probability;
	};
} // namespace sem

