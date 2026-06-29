// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file CellObj.h
 * @brief This file contains the declaration of the CellObj class.
 */
#pragma once

#include <string>
#include <vector>


#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>


namespace tdb {

/**
	* @class CellObj
	* @brief A cell in a grid can hold a list of objects. This class is used to hold list of object unique identifiers in a cell.
	*/
class CellObj {
public:

	/**
		* @brief This function is used to serialize the cell object.
		* 
		* @param[in] ar The archive to serialize the cell object
		* @param[in] version The version of the serialization (not used)
		*/
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		auto count = m_objUIDs.size();
		//ar& BOOST_SERIALIZATION_NVP(count);
		ar& BOOST_SERIALIZATION_NVP(m_objUIDs);			
	}

	/**
		* @brief This function is used to deserialize the cell object.
		* 
		* @param[in] ar The archive to deserialize the cell object
		* @param[in] version The version of the serialization (not used)
		*/
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		//TBD
		ar& BOOST_SERIALIZATION_NVP(m_objUIDs);
	}

	void add(const std::string& uid) { m_objTypes.push_back(uid); }

	inline const auto& getObjUIDS() const { return m_objUIDs; }

	BOOST_SERIALIZATION_SPLIT_MEMBER()
private:		
	std::vector<std::string> m_objUIDs;
	std::vector<std::string> m_objTypes; //TBD: enum
};


} //namespace tdb
