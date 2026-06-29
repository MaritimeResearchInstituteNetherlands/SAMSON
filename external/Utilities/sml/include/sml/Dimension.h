// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file Dimension.h
 * @brief This file contains the declaration of the Dimension2D struct.
 */
#pragma once

#include <utility>
#include <string_view>

namespace sml {
	
/**
	* @struct Dimension2D
	* @brief Struct to define the dimension of an object in 2D space in meters.
	*/
struct Dimension2D {
	double x;
	double y;

	/**
		* @brief This function is used to serialize and deserialize the Dimension2D object.
		* 
		* @param[in] ar The archive to serialize and deserialize the Dimension2D object
		* @param[in] version The version of the serialization (not used)
		*/
	//template<class Archive>
	//void serialize(Archive& ar, const unsigned int)
	//{
	//	ar& BOOST_SERIALIZATION_NVP(x);
	//	ar& BOOST_SERIALIZATION_NVP(y);		
	//}
};

} //namespace sml
