// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "util/IIdentifiable.h" // TODO move this to a standalone database library
#include <tdb/TrafficSchemes.h>

namespace sem
{

class ShipLinkDataScheme : public tdb::IIdentifiable
{
public:
	ShipLinkDataScheme();

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("frequency", m_frequency);
		ar& boost::serialization::make_nvp("speed", m_speed);
		ar& boost::serialization::make_nvp("mu", m_mu);
		ar& boost::serialization::make_nvp("sigma", m_sigma);
	}

	inline const std::string getUID() const override { return m_uid; }

	inline const double getShipRate() const { return m_frequency; }

	inline const double getSpeed() const { return m_speed; }

	inline const double getMu() const { return m_mu; }

	inline const double getSigma() const { return m_sigma == 0 ? 1e-9 : m_sigma; } // Regularisation of sigma, zero is not allowed in computations

	using UidType = std::string;
private:
	UidType m_uid;
	double m_frequency;
	double m_speed;
	double m_mu;
	double m_sigma;
};



class ShipCellDataScheme : public tdb::IIdentifiable
{
public:
	ShipCellDataScheme();

	inline const std::string getUID() const override { return m_uid; }
	/**
	* @brief Function to serialize and deserialize the ship link object.
	*
	* @param[in] ar The archive to serialize and deserialize the ship link object
	* @param[in] version The version of the serialization (not used)
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& boost::serialization::make_nvp("uid", m_uid);
		ar& boost::serialization::make_nvp("density", m_density);
		ar& boost::serialization::make_nvp("speed", m_speed);
		ar& boost::serialization::make_nvp("direction", m_direction);
	}

	inline double getDensity() const { return m_density; }

	inline double getSpeed() const { return m_speed; }

	inline double getDirection() const { return m_direction; }

	using UidType = std::string;
private:
	UidType m_uid;
	double m_density;
	double m_speed;
	double m_direction;

};
} // namespace sem

