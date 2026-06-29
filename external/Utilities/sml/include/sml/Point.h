// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file Point.h
 * @brief This file contains the declaration of the Point2D struct.
 */

#pragma once

#include <sml/Coordinate.h>
#include <sml/Dimension.h>

#include <utility>
#include <string_view>

//namespace sml {
//
///**
//	* @struct Point2D
//	* 
//	* @brief Struct to define a point in 2D space. 
//	* 
//	* The point is defined by its x and y coordinates in the 2D space. 
//	* TBD: Currently, point is used as a vector in the 2D space as well. This should be separated in the future.
//	* 
//	* @details 	
// 	* It is important to note that in this struct:
// 	* - `x` corresponds to `lon` (longitude).
// 	* - `y` corresponds to `lat` (latitude).	 
//	* This distinction is crucial because developers often refer to cartesian coordinates in order of `x` and `y`, and geographical coordinates as `lat` and `lon`.
// 	* Misunderstanding this mapping can lead to confusion and make the code difficult to follow.
// 	* 
// 	* Therefore, always ensure that when you see `x` and `y` in this context, they are interpreted as `lon` and `lat` respectively in the context of world coordinates.
//	*/
//// TODO refactor point, as it behaves like a Euclidian vector
////struct Point2D {
////	double x; // lon 
////	double y; // lat
//
//	/**
//		* @brief Function to serialize and deserialize the point object.
//		* 		 
//		* 
//		* @param[in] ar The archive to serialize and deserialize the point object
//		* @param[in] version The version of the serialization (not used)
//		*/
//	//template<class Archive>
//	//void serialize(Archive& ar, const unsigned int)
//	//{
//	//	ar& BOOST_SERIALIZATION_NVP(x);
//	//	ar& BOOST_SERIALIZATION_NVP(y);
//	//}
//
//	/**
//		* @brief Function to add a dimension to the point.
//		* 
//		* @param[in] dimension The dimension to add to the point.
//		* @return The point with the added dimension.
//		*/
//	//Point2D operator+(const Dimension2D& dimension) const;
//
//	/**
//		* @brief Function to subtract a point from the point.
//		* 
//		* @param[in] other The point to subtract from the point.
//		* @return The point after subtracting the other point.
//		*/
//	//Point2D operator-(const Point2D& other) const;
//
//	/**
//		* @brief Function to get the dot product of the point with another point.
//		* 
//		* @param[in] other The point to multiply with the point.
//		* @return The scalar value of the dot product.
//		*/
//	//double dot(const Point2D& other) const;
//
//	/**
//		* @brief Function to get the perpendicular of the point.
//		* 
//		* This function returns the perpendicular of the point, which is a vector perpendicular to the point/vector.
//		* 
//		* @return The perpendicular of the point.
//		*/
//	//Point2D perpendicular() const;
//
//	/**
//		* @brief Function to normalize the vector.
//		* 
//		* @return The normalized vector.
//		*/
//	//Point2D normalize() const;
//
//	/**
//		* @brief Function to get the magnitude of the vector.
//		* 
//		* @return The magnitude of the vector.
//		*/
//	//double magnitude() const;
//};

//} //namespace sml
