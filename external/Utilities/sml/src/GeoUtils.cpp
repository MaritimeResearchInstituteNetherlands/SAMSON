// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sml/GeoUtils.h>
#include <sml/Vector.h>

#include <cmath>
#include <numbers> 



namespace sml {

/*
* Assuming input coordinates has sign of:
*  Northern latitudes from the Equator are positive, while southern latitudes are negative.
*  Longitudes east of the Prime Meridian are positive, while longitudes west are negative
*/
void GeoUtils::updateOrientation(Coordinate2D& start, Coordinate2D& end, const OriginDirFrom& originDirection) {

	if (originDirection.latDir == LatDir::South) {
		if (start.lat > end.lat) {
			std::swap(start.lat, end.lat);
		}
	}
	else {
		if (start.lat < end.lat) {
			std::swap(start.lat, end.lat);
		}
	}

	if (originDirection.lonDir == LonDir::West) {
		if (start.lon > end.lon) {
			std::swap(start.lat, end.lat);
		}
	}
	else {
		if (start.lon < end.lon) {
			std::swap(start.lon, end.lon);
		}
	}
}


//Point2D GeoUtils::toPoint(const Coordinate2D& origin, const Coordinate2D& coord, const OriginDirFrom& originDirection)
//{
//	auto pointLat_y = haversineDistanceWGS84(origin.lat, origin.lon, coord.lat, origin.lon);
//	auto pointLon_x = haversineDistanceWGS84(origin.lat, origin.lon, origin.lat, coord.lon);
//
//	auto latDirCoef = -1;
//	if (originDirection.latDir == LatDir::South) {
//		latDirCoef = 1;
//	}
//	pointLat_y = (coord.lat > origin.lat) ? pointLat_y * latDirCoef : -pointLat_y * latDirCoef;
//
//	auto lonDirCoef = -1;
//	if (originDirection.lonDir == LonDir::West) {
//		lonDirCoef = 1;
//	}
//	pointLon_x = (coord.lon > origin.lon) ? pointLon_x * lonDirCoef : -pointLon_x * lonDirCoef;
//
//	return { .x = pointLon_x , .y = pointLat_y };
//}



double GeoUtils::distance(const Coordinate2D& a, const Coordinate2D& b)
{
	return std::abs(haversineDistanceWGS84(a, b));
}



//Point2D GeoUtils::toPoint(const Coordinate2D& origin, const Coordinate2D& coord)
//{
//	auto pointLat_y = haversineDistanceWGS84(origin.lat, origin.lon, coord.lat, origin.lon);
//	auto pointLon_x = haversineDistanceWGS84(origin.lat, origin.lon, origin.lat, coord.lon);
//
//	pointLat_y = (coord.lat > origin.lat) ? pointLat_y : -pointLat_y;
//	pointLon_x = (coord.lon > origin.lon) ? pointLon_x : -pointLon_x;
//
//	return { .x = pointLon_x , .y = pointLat_y };
//}


// TODO sort out what kind of projection this is, perhaps with some references or something
// TODO what about numerical stability, 
// see https://sebastianof.github.io/GeoDsBlog/posts/gds-2024-01-10-haversine-dist/#:~:text=plt.show()-,Two%20Points'%20Distance,a%20straight%20tunnel%20connecting%20and%20.
Vector2d GeoUtils::projectToPlane(const Coordinate2D& origin, const Coordinate2D& coordinate)
{
	double pointLat_y = haversineDistanceWGS84(origin.lat, origin.lon, coordinate.lat, origin.lon);
	double pointLon_x = haversineDistanceWGS84(origin.lat, origin.lon, origin.lat, coordinate.lon);

	pointLat_y = (coordinate.lat > origin.lat) ? pointLat_y : -pointLat_y;
	pointLon_x = (coordinate.lon > origin.lon) ? pointLon_x : -pointLon_x;


	return Vector2d(pointLon_x, pointLat_y);
}



Coordinate2D GeoUtils::projectionToCoordinate(const Coordinate2D& origin, const Vector2d position)
{
	double lat = inverseHaversineDistanceWGS84(origin, position[1], 0);
	double lon = inverseHaversineDistanceWGS84(origin, position[0], 1);
	return Coordinate2D(lat, lon);
}



//double GeoUtils::calculateIntersectionAngle(const Point2D& line1P1, const Point2D& line1P2,
//	const Point2D& line2P1, const Point2D& line2P2) {
//
//	Point2D v = line1P2 - line1P1;
//	Point2D w = line2P2 - line2P1;
//
//	auto radian = atan2(w.y * v.x - w.x * v.y, w.x * v.x + w.y * v.y);
//	auto degree = radian * GeoUtils::toDegree;
//	if (degree < 0) {
//		degree += 360.0;
//	}
//	return degree;
//}

//double GeoUtils::calculateIntersectionAngleDotProduct(const Point2D& line1P1, const Point2D& line1P2, const Point2D& line2P1, const Point2D& line2P2)
//{
//	Point2D v = line1P2 - line1P1;
//	Point2D w = line2P2 - line2P1;
//
//	double dot = v.dot(w);
//	double magV = v.magnitude();
//	double magW = w.magnitude();
//
//	double cosTheta = dot / (magV * magW);
//
//	//fix possible floating point error
//	cosTheta = std::max(-1.0, std::min(1.0, cosTheta));
//
//	double radian = std::acos(cosTheta);
//	double degree = radian * GeoUtils::toDegree;
//	return degree;
//}


//bool GeoUtils::doLinesIntersect(const Point2D& line1P1, const Point2D& line1P2, const Point2D& line2P1, const Point2D& line2P2) {
//	float q = (line1P1.y - line2P1.y) * (line2P2.x - line2P1.x) - (line1P1.x - line2P1.x) * (line2P2.y - line2P1.y);
//	float d = (line1P2.x - line1P1.x) * (line2P2.y - line2P1.y) - (line1P2.y - line1P1.y) * (line2P2.x - line2P1.x);
//
//	if (d == 0) {
//		return false; // Parallel lines
//	}
//
//	float r = q / d;
//	q = (line1P1.y - line2P1.y) * (line1P2.x - line1P1.x) - (line1P1.x - line2P1.x) * (line1P2.y - line1P1.y);
//	float s = q / d;
//	return !(r < 0 || r > 1 || s < 0 || s > 1);
//}

//bool GeoUtils::isPointInCell(const Point2D& p, const Cell2D& cell, const Dimension2D& dimension)
//{
//	auto endPoint = cell.start + dimension;
//	return p.x >= cell.start.x && p.x <= endPoint.x && p.y >= cell.start.y && p.y <= endPoint.y;
//}

//Checks if one the points is in the cell first, if not check line segment intersection of each edges of the cell with given line
//bool GeoUtils::doesLineInOrIntersectCell(const Point2D& line1P1, const Point2D& line1P2, const Cell2D& cell, const Dimension2D& dimension)
//{
//
//	return (GeoUtils::isPointInCell(line1P1, cell, dimension) || //if the start point of the line is in, the lne is for sure in the cell
//		GeoUtils::isPointInCell(line1P2, cell, dimension) ||  //if the end point of the line is in, the lne is for sure in the cell
//		GeoUtils::doLinesIntersect(line1P1, line1P2, cell.start, { cell.start.x + dimension.x, cell.start.y }) || //Bottom edge
//		GeoUtils::doLinesIntersect(line1P1, line1P2, { cell.start.x + dimension.x , cell.start.y }, { cell.start.x + dimension.x, cell.start.y + dimension.y }) || //Right edge
//		GeoUtils::doLinesIntersect(line1P1, line1P2, { cell.start.x + dimension.x , cell.start.y + dimension.y }, { cell.start.x , cell.start.y + dimension.y }) || //Top edge
//		GeoUtils::doLinesIntersect(line1P1, line1P2, { cell.start.x , cell.start.y + dimension.y }, cell.start)); //Top edge
//
//}


//std::pair<size_t, size_t> GeoUtils::getCellOfPoint(const Point2D& p, const Dimension2D dimension) {
//
//	return std::make_pair<size_t, size_t>(static_cast<size_t>(p.x / dimension.x), static_cast<size_t>(p.y / dimension.y));
//}


double GeoUtils::haversineDistanceWGS84(double lat1, double lon1, double lat2, double lon2) {
	const double dLat = (lat2 - lat1) * toRadian;
	const double dLon = (lon2 - lon1) * toRadian;
	const double rLat1 = lat1 * toRadian;
	const double rLat2 = lat2 * toRadian;
	const double c = asin(sqrt(sin(dLat / 2) * sin(dLat / 2) + cos(rLat1) * cos(rLat2) * sin(dLon / 2) * sin(dLon / 2)));
	return 2 * R_WG84_meter * c; // Distance in meters
}



double GeoUtils::haversineDistanceWGS84(const sml::Coordinate2D& a, const sml::Coordinate2D& b)
{
	return haversineDistanceWGS84(a.lat, a.lon, b.lat, b.lon);
}


// direction = 0 -> lat
// direction = 1 -> lon
// Check if we need to support other directions
double GeoUtils::inverseHaversineDistanceWGS84(const sml::Coordinate2D& origin, double distance, int direction)
{
	// dir = 0: lon2 = lon1, dlon = 0
	if (direction == 0) {
		double dlat = distance / R_WG84_meter;
		double lat2 = dlat * toDegree + origin.lat;
		return lat2;
	}
	else if (direction == 1) {
		// dir = 1, lat2 = lat1, dlat = 0 rlat1 = rlat2
		auto rLat = origin.lat * toRadian;
		double dLon = 2.0 * asin(sin(distance / (2.0 * R_WG84_meter)) / cos(rLat));
		double lon2 = dLon * toDegree + origin.lon;
		return lon2;
	}
	else {
		std::cout << "Calling unsupported function. " << std::endl;
		return 0.0;
	}
}
} // namespace sml
