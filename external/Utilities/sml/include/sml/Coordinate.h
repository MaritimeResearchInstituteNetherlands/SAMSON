// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file Coordinate.h
 * 
 * @brief This file contains the declaration of the Coordinate2D struct and the OriginDirFrom struct.
 */
 

#pragma once

//#include <sml/Point.h>

#include <sml_export.h>
#include <utility>
#include <string_view>
#include <sstream>
#include <iostream>

namespace sml {

/**
	* @enum LonDir 
	* @brief Enum to define the direction of longitude in the coordinate system.
	*/
enum class LonDir : char {
	East = 'E',
	West = 'W'
};



/**
	* @enum LatDir
	* @brief Enum to define the direction of latitude in the coordinate system.
	*/
enum class LatDir : char {
	North = 'N',
	South = 'S'
};
	


/**
	* @struct OriginDirFrom
	* @brief Struct to define the origin of the coordinate system.
	*/
struct OriginDirFrom {
	LonDir lonDir;
	LatDir latDir;
};



/**
	* @struct Coordinate2D
	* @brief Struct to define a 2D coordinate system.
	*/
struct Coordinate2D {
	double lat;
	double lon;
};


} //namespace sml

inline std::ostream& operator<<(std::ostream& s, const sml::Coordinate2D& coordinate)
{
	s << "{" << coordinate.lat << ", " << coordinate.lon << "}";
	return s;
}
