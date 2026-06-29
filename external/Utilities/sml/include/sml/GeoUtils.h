// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file GeoUtils.h
 * 
 * @brief This file contains the declaration of the GeoUtils class.
 */

#pragma once

#include <sml_export.h>

#include <sml/Point.h>
#include <sml/Coordinate.h>
#include <sml/Cell.h>
#include <sml/Dimension.h>
#include <sml/Vector.h>

#include <numbers>

namespace sml {
/**
	* @class GeoUtils
	* 
	* @brief This class contains static utility functions for geometric calculations.
	*/
class SML_EXPORT GeoUtils {
public:

	/**
		* @brief Calculates the distance between two points in meters.
		* 
		* This function uses the Haversine formula to determine the great-circle distance between 
		* the origin and destination points. The origin point is often evaluated as (0,0) in the 
		* cartesian coordinate system, making the distance to the destination point its position 
		* in the cartesian plane.
		* 
		* @param[in] origin The origin point in the coordinate system.
		* @param[in] coord The destination point in the coordinate system.
		* @param[in] originDirection The direction from the origin point, used to determine the sign of the distance.
		* @return The distance between the two points in meters.
		*/
	//static Point2D toPoint(const Coordinate2D& origin, const Coordinate2D& coord, const OriginDirFrom& originDirection);

	static double distance(const Coordinate2D& a, const Coordinate2D& b);

	static Vector2d projectToPlane(const Coordinate2D& origin, const Coordinate2D& coordinate);

	static Coordinate2D projectionToCoordinate(const Coordinate2D& origin, const Vector2d position);
		
		

		/**
		* @brief Calculates the distance between two points in meters.
		* 
		* This function uses the Haversine formula to determine the great-circle distance between 
		* the origin and destination points. The origin point is often evaluated as (0,0) in the 
		* cartesian coordinate system, making the distance to the destination point its position 
		* in the cartesian plane.
		* 
		* Assumes direction is South and West to determine the relative position of the points.
		* 
		* @param[in] origin The origin point in the coordinate system.
		* @param[in] coord The destination point in the coordinate system.
		* @return The distance between the two points in meters.
		*/
	/**
		* @brief This function calculates the distance between two points in meters.
		* 
		 
		* 
		* @param[in] origin The origin point
		* @param[in] coord The destination point
		*/
	//static Point2D toPoint(const Coordinate2D& origin, const Coordinate2D& coord);
		
	/**
		* @brief This function creates a proper start and end point for an area regarding the origin direction. 
		* 
		* The start point in an area is always the bottom-left point and the end point is the top-right point regarding the origin direction. 
		* 
		* Assuming input coordinates has sign of: 
		* 		Northern latitudes from the Equator are positive, while southern latitudes are negative.
		* 		Longitudes east of the Prime Meridian are positive, while longitudes west are negative
		* 
		* @param[in,out] start The start point of the area to be updated
		* @param[in,out] end The end point of the area to be updated
		* @param[in] originDirection The direction from the origin point, used to determine the swap of the start and end points
		*/
	static void updateOrientation(Coordinate2D& start, Coordinate2D& end, const OriginDirFrom& originDirection = { LonDir::West, LatDir::South });

	/**
		* @brief This function calculates the angle between two lines, assuming they intersect.
		* 
		* Calculation is based on atan2 function. The angle is calculated relative to line1. Returned value is normalized between 0 and 360 degrees.
		* 
		* @param[in] line1P1 The first point of the first line
		* @param[in] line1P2 The second point of the first line
		* @param[in] line2P1 The first point of the second line
		* @param[in] line2P2 The second point of the second line
		* @return The angle between the two lines in degrees
		*/
	//static double calculateIntersectionAngle(const Point2D& line1P1, const Point2D& line1P2, const Point2D& line2P1, const Point2D& line2P2);


	/**
		* @brief This function calculates the angle between two lines, assuming they intersect.
		* 
		* Calculation is based on dot product. Returned value is between 0 and 180 degrees.
		* 
		* @param[in] line1P1 The first point of the first line
		* @param[in] line1P2 The second point of the first line
		* @param[in] line2P1 The first point of the second line
		* @param[in] line2P2 The second point of the second line
		* @return The angle between the two lines in degrees
		*/
	//static double calculateIntersectionAngleDotProduct(const Point2D& line1P1, const Point2D& line1P2, const Point2D& line2P1, const Point2D& line2P2);

	/**
		* @brief This function checks if two line segments intersect.
		* 
		* Line segments intersect algorithm is based on cross product. The main idea is to check if the end points of the line segments are on the opposite sides of the other line segment.
		* 
		*/
	//Line segment intersection algorithm based on cross product

	/**
		* @brief Determines if two line segments intersect.
		* 
		* This method checks whether two line segments, defined by their endpoints, intersect.
		* This method is based on parametric equation of the line segments: 
		* https://gamedev.stackexchange.com/questions/44720/line-intersection-from-parametric-equation
		* 
		* @param line1P1 The first endpoint of the first line segment.
		* @param line1P2 The second endpoint of the first line segment.
		* @param line2P1 The first endpoint of the second line segment.
		* @param line2P2 The second endpoint of the second line segment.
		* @return true if the line segments intersect, false otherwise.
		*/
	//static bool doLinesIntersect(const Point2D& line1P1, const Point2D& line1P2, const Point2D& line2P1, const Point2D& line2P2);

	/**
		* @brief Determines if a point is inside a cell.
		* 
		* This method checks whether a point is inside a cell defined by its bottom-left corner and its dimension.
		* 
		* @param p The point to check.
		* @param cell The cell to check.
		* @param dimension The dimension of the cell.
		*/
	//static bool isPointInCell(const Point2D& p, const Cell2D& cell, const Dimension2D& dimension);

	/**
		* @brief Determines if a line is inside a cell or intersects it.
		* 
		* This method checks whether a line defined by its endpoints is inside a cell or intersects it.
		* First check if one of the points is in the cell, if not check line segment intersection of each edges of the cell with given line.
		* 
		* @param line1P1 The first endpoint of the line.
		* @param line1P2 The second endpoint of the line.
		* @param cell The cell to check.
		* @param dimension The dimension of the cell.
		* @return true if the line is inside the cell or intersects it, false otherwise.
		*/		
	//static bool doesLineInOrIntersectCell(const Point2D& line1P1, const Point2D& line1P2, const Cell2D& cell, const Dimension2D& dimension);

	/**
		* @brief Determines the cell index of a point in a grid.
		* 
		* This method determines the cell index of a point in a grid defined by cell dimension.
		* 
		* @param p The point to determine the cell index.
		* @param dimension The dimension of a cell in the grid.
		*/
	//static std::pair<size_t, size_t> getCellOfPoint(const Point2D& p, const Dimension2D dimension);


private:

	/**
		* @brief This function calculates the distance between two points in meters.
		* 
		* The haversine formula determines the great-circle distance between two points on a sphere given their longitudes and latitudes.
		* https://rosettacode.org/wiki/Haversine_formula#C++
		* 
		* @param[in] lat1 The latitude of the first point
		* @param[in] lon1 The longitude of the first point
		* @param[in] lat2 The latitude of the second point
		* @param[in] lon2 The longitude of the second point
		* @return The distance between the two points in meters
		*/		
	static double haversineDistanceWGS84(double lat1, double lon1, double lat2, double lon2);

	static double haversineDistanceWGS84(const sml::Coordinate2D& a, const sml::Coordinate2D& b);

	static double inverseHaversineDistanceWGS84(const sml::Coordinate2D& origin, double distance, int direction);

	static constexpr double R_WG84_meter = 6371000.0;//Earth of radius in meters
	static constexpr double R_meter = 6378137.0; //Equatorial radius of the earth in meters
public:
	static constexpr double toRadian = std::numbers::pi / 180.0; 
	static constexpr double toDegree= 180.0 / std::numbers::pi;
};
} //namespace sml 
