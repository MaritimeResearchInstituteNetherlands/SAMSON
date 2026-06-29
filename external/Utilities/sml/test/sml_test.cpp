// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <gtest/gtest.h>

#include <sml/Vector.h>
#include <sml/Polygon.h>
#include <sml/PolyLine.h>
#include <sml/Functions.h>
#include <sml/AngleBounds.h>
#include <sml/Array3D.h>
#include <sml/Array4D.h>
#include <sml/Array5D.h>
#include <sml/AngularDistributionFunction.h>
#include <sml/Segment.h>
#include <sml/GeoUtils.h>
#include <chrono>
#include <memory>
#include <vector>

#define CURRENT_TEST_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static double accuracy = 1e-7;

TEST(sml_test, array4d)
{
	sml::Array4D<double> array;
	array.resize(2, 3, 4, 5);
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 3; j++) {
			for (size_t k = 0; k < 4; k++) {
				for (size_t l = 0; l < 5; l++) {
					array[i][j][k][l] = i * j * k * l;
				}
			}
		}
	}
}



TEST(sml_test, array5d)
{
	sml::Array5D<double> array;
	array.resize(2, 3, 4, 5,6);
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 3; j++) {
			for (size_t k = 0; k < 4; k++) {
				for (size_t l = 0; l < 5; l++) {
					for (size_t m = 0; m < 6; m++) {
						array[i][j][k][l][m] = i * j * k * l * m;
					}
				}
			}
		}
	}
}



TEST(sml_test, vector_constructor_default) 
{
	sml::Vector3d vector;
	EXPECT_EQ(vector[0], 0.0);
	EXPECT_EQ(vector[1], 0.0);
	EXPECT_EQ(vector[2], 0.0);
}



TEST(sml_test, vector_constructor_initialise) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	EXPECT_EQ(vector[0], 1.1);
	EXPECT_EQ(vector[1], 2.2);
	EXPECT_EQ(vector[2], 3.3);
	
	sml::Vector3d vector2 = { 1.1, 2.2, 3.3 };
	EXPECT_EQ(vector2[0], 1.1);
	EXPECT_EQ(vector2[1], 2.2);
	EXPECT_EQ(vector2[2], 3.3);
	
	sml::Vector3d vector3 = { 1.1, 2.2};
	EXPECT_EQ(vector3[0], 1.1);
	EXPECT_EQ(vector3[1], 2.2);
	EXPECT_EQ(vector3[2], 0.0);
}



TEST(sml_test, vector_constructor_copy) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	EXPECT_EQ(vector[0], 1.1);
	EXPECT_EQ(vector[1], 2.2);
	EXPECT_EQ(vector[2], 3.3);
	
	sml::Vector3d vector2(vector);
	EXPECT_EQ(vector2[0], 1.1);
	EXPECT_EQ(vector2[1], 2.2);
	EXPECT_EQ(vector2[2], 3.3);
}



TEST(sml_test, vector_constructor_move) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	EXPECT_EQ(vector[0], 1.1);
	EXPECT_EQ(vector[1], 2.2);
	EXPECT_EQ(vector[2], 3.3);
	
	sml::Vector3d vector2(std::move(vector));
	EXPECT_EQ(vector2[0], 1.1);
	EXPECT_EQ(vector2[1], 2.2);
	EXPECT_EQ(vector2[2], 3.3);
}



TEST(sml_test, vector_assignment_copy) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	EXPECT_EQ(vector[0], 1.1);
	EXPECT_EQ(vector[1], 2.2);
	EXPECT_EQ(vector[2], 3.3);

	sml::Vector3d vector2(4.4, 5.5, 6.6);
	EXPECT_EQ(vector2[0], 4.4);
	EXPECT_EQ(vector2[1], 5.5);
	EXPECT_EQ(vector2[2], 6.6);

	vector2 = vector;
	EXPECT_EQ(vector[0], 1.1);
	EXPECT_EQ(vector[1], 2.2);
	EXPECT_EQ(vector[2], 3.3);
	EXPECT_EQ(vector2[0], 1.1);
	EXPECT_EQ(vector2[1], 2.2);
	EXPECT_EQ(vector2[2], 3.3);
}



TEST(sml_test, vector_assignment_move) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	EXPECT_EQ(vector[0], 1.1);
	EXPECT_EQ(vector[1], 2.2);
	EXPECT_EQ(vector[2], 3.3);

	sml::Vector3d vector2(4.4, 5.5, 6.6);
	EXPECT_EQ(vector2[0], 4.4);
	EXPECT_EQ(vector2[1], 5.5);
	EXPECT_EQ(vector2[2], 6.6);

	vector2 = std::move(vector);
	EXPECT_EQ(vector[0], 4.4);
	EXPECT_EQ(vector[1], 5.5);
	EXPECT_EQ(vector[2], 6.6);
	EXPECT_EQ(vector2[0], 1.1);
	EXPECT_EQ(vector2[1], 2.2);
	EXPECT_EQ(vector2[2], 3.3);
}



TEST(sml_test, vector_value) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	vector[0] = 4.4;
	vector[1] = 5.5;
	vector[2] = 6.6;

	EXPECT_EQ(vector[0], 4.4);
	EXPECT_EQ(vector[1], 5.5);
	EXPECT_EQ(vector[2], 6.6);
}



TEST(sml_test, vector_addition) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	sml::Vector3d vector2(1.1, 2.2, 3.3);
	sml::Vector3d vector3(1.2, 2.3, 3.4);

	vector2 += vector;

	EXPECT_NEAR(vector2[0], 2.2, accuracy);
	EXPECT_NEAR(vector2[1], 4.4, accuracy);
	EXPECT_NEAR(vector2[2], 6.6, accuracy);

	sml::Vector3d vector4 = vector + vector2 + vector3;
	EXPECT_NEAR(vector4[0], 4.5, accuracy);
	EXPECT_NEAR(vector4[1], 8.9, accuracy);
	EXPECT_NEAR(vector4[2], 13.3, accuracy);
}



TEST(sml_test, vector_subtraction) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	sml::Vector3d vector2(1.1, 2.2, 3.3);

	vector2 -= vector;

	EXPECT_EQ(vector2[0], 0.0);
	EXPECT_EQ(vector2[1], 0.0);
	EXPECT_EQ(vector2[2], 0.0);

	sml::Vector3d vector3 = vector2 - vector - vector;
	EXPECT_NEAR(vector3[0], -2.2, accuracy);
	EXPECT_NEAR(vector3[1], -4.4, accuracy);
	EXPECT_NEAR(vector3[2], -6.6, accuracy);
}



TEST(sml_test, vector_scale)
{
	sml::Vector3d vector{ 1.1, 2.2, 3.3 };
	sml::Vector3d vector2{ 2.2, 4.4, 6.6 };

	sml::Vector3d vector3 = vector * 2.0;
	EXPECT_NEAR(vector3[0], vector2[0], accuracy);
	EXPECT_NEAR(vector3[1], vector2[1], accuracy);
	EXPECT_NEAR(vector3[2], vector2[2], accuracy);
}



TEST(sml_test, vector_inPlacedivision) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	vector /= 0.5;
	EXPECT_NEAR(vector[0], 2.2, accuracy);
	EXPECT_NEAR(vector[1], 4.4, accuracy);
	EXPECT_NEAR(vector[2], 6.6, accuracy);
}



TEST(sml_test, vector_division)
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	sml::Vector3d result = vector / 1.1;
	EXPECT_NEAR(result[0], 1.0, accuracy);
	EXPECT_NEAR(result[1], 2.0, accuracy);
	EXPECT_NEAR(result[2], 3.0, accuracy);
}



TEST(sml_test, vector_dot) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	sml::Vector3d vector2(1.1, 2.2, 3.3);

	double dot = vector * vector2;
	double result = vector[0] * vector2[0] + vector[1] * vector2[1] + vector[2] * vector2[2];
	EXPECT_EQ(dot, result);
}



TEST(sml_test, vector_norm) 
{
	sml::Vector3d vector(1.1, 2.2, 3.3);


	double norm = vector.norm();
	double result = std::sqrt(1.1 * 1.1 + 2.2 * 2.2 + 3.3 * 3.3);
	EXPECT_EQ(norm, result);
}



TEST(sml_test, vector_normalise)
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	sml::Vector3d vector2 = vector;
	vector2 /= vector.norm();
	vector.normalise();

	EXPECT_NEAR(vector[0], vector2[0], accuracy);
	EXPECT_NEAR(vector[1], vector2[1], accuracy);
	EXPECT_NEAR(vector[2], vector2[2], accuracy);
}



TEST(sml_test, vector_negate)
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	sml::Vector3d vector2 = -vector;
	EXPECT_EQ(vector2[0], -vector[0]);
	EXPECT_EQ(vector2[1], -vector[1]);
	EXPECT_EQ(vector2[2], -vector[2]);
}



TEST(sml_test, vector_equal)
{
	sml::Vector3d vector(1.1, 2.2, 3.3);
	sml::Vector3d vector2(1.1, 2.2, 3.3);
	sml::Vector3d vector3(1.1, 2.2, 3.3 + 1e-6);
	EXPECT_TRUE(vector == vector2);
	EXPECT_TRUE(vector != vector3);
}



TEST(sml_test, cfd_normalDistribution)
{
	double accuracy = 1e-8;
	double p;
	double mu_to_sigma = 0.34134474605;

	// Check mean probability
	p = sml::cdf_normalDistribution(0.0, 0.0, 2.5);
	EXPECT_LT(std::abs(p - 0.5), accuracy);
	p = sml::cdf_normalDistribution(1.0, 1.0, 2.5);
	EXPECT_LT(std::abs(p - 0.5), accuracy);

	// Check standard deviation probability
	p = sml::cdf_normalDistribution(2.5, 0.0, 2.5);
	EXPECT_LT(std::abs(p - (0.5 +  mu_to_sigma)), accuracy);
	p = sml::cdf_normalDistribution(-2.5, 0.0, 2.5);
	EXPECT_LT(std::abs(p - (0.5 - mu_to_sigma)), accuracy);
	p = sml::cdf_normalDistribution(3.5, 1.0, 2.5);
	EXPECT_LT(std::abs(p - (0.5 + mu_to_sigma)), accuracy);
	p = sml::cdf_normalDistribution(-1.5, 1.0, 2.5);
	EXPECT_LT(std::abs(p - (0.5 - mu_to_sigma)), accuracy);
}



TEST(sml_test, projection)
{
	// Generate two coordinates
	sml::Coordinate2D origin(5.0, 6.1);
	sml::Coordinate2D target(1.0, 2.0);

	// Project onto a plane using the other origin
	sml::Vector2d vectorTarget = sml::GeoUtils::projectToPlane(origin, target);
	sml::Vector2d vectorOrigin = sml::GeoUtils::projectToPlane(target, origin);

	// The projection of one point with the origin of the otherpoint generate different carthesian projections
	// due to the lattitude of the origin. The resulting positions are close, but not exactly equal, something
	// to be sorted out. The error made is probably small
	sml::Vector2d sum = (vectorTarget + vectorOrigin) / vectorTarget.norm();
	EXPECT_LT(sum.norm(), 1e-2);

	// Transform back to the coordinate
	sml::Coordinate2D target2 = sml::GeoUtils::projectionToCoordinate(origin, vectorTarget);
	sml::Coordinate2D origin2 = sml::GeoUtils::projectionToCoordinate(target, vectorOrigin);
	EXPECT_EQ(origin.lat, origin2.lat);
	EXPECT_EQ(origin.lon, origin2.lon);
	EXPECT_EQ(target.lat, target2.lat);
	EXPECT_EQ(target.lon, target2.lon);
}



TEST(sml_test, angleBounds_inRange)
{
	sml::AngleBounds bounds;
	bounds.setLowerBound(5.497787143782138);
	bounds.setUpperBound(0.3217505543966423);
	bounds.setExtent(1.1071487177940904);

	EXPECT_TRUE(bounds.isInRange(bounds.getLowerBound()));
	EXPECT_TRUE(bounds.isInRange(bounds.getUpperBound()));
	EXPECT_FALSE(bounds.isInRange(bounds.getLowerBound(), false));
	EXPECT_FALSE(bounds.isInRange(bounds.getUpperBound(), false));
	EXPECT_TRUE(bounds.isInRange(0.1));
	EXPECT_TRUE(bounds.isInRange(0.0));
	EXPECT_TRUE(bounds.isInRange(-0.1));
	EXPECT_FALSE(bounds.isInRange(-2.0));
	EXPECT_FALSE(bounds.isInRange(5.4));
}



TEST(sml_test, angleBounds_a)
{
	// Simple test data
	sml::Coordinate2D origin(5.0, 6.1);
	sml::Vector2d p0 = { -1.0, 1.0};
	sml::Vector2d p1 = { 1.0, 1.0 };

	// Rectangle above line
	std::vector<sml::Coordinate2D> coordinates0;
	coordinates0.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, 4.0)));
	coordinates0.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(1.0, 4.0)));
	coordinates0.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(1.0, 5.0)));
	coordinates0.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, 5.0)));
	sml::AngleBounds solution0;
	solution0.setLowerBound(0.982793723247329);
	solution0.setUpperBound(2.158798930342464);
	solution0.setExtent(1.176005207095135);

	// Rectangle infront of line
	std::vector<sml::Coordinate2D> coordinates1;
	coordinates1.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(4.0, 2.0)));
	coordinates1.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(5.0, 2.0)));
	coordinates1.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(5.0, -2.0)));
	coordinates1.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(4.0, -2.0)));
	sml::AngleBounds solution1;
	solution1.setLowerBound(5.497787143782138);
	solution1.setUpperBound(0.3217505543966423);
	solution1.setExtent(1.1071487177940904);

	// Rectangle below line
	std::vector<sml::Coordinate2D> coordinates2;
	coordinates2.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, -2.0)));
	coordinates2.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(1.0, -2.0)));
	coordinates2.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(1.0, -3.0)));
	coordinates2.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, -3.0)));
	sml::AngleBounds solution2;
	solution2.setLowerBound(solution0.getLowerBound() + std::numbers::pi);
	solution2.setUpperBound(solution0.getUpperBound() + std::numbers::pi);
	solution2.setExtent(solution0.getExtent());

	// Rectangle behind line
	std::vector<sml::Coordinate2D> coordinates3;
	coordinates3.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-4.0, 2.0)));
	coordinates3.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-5.0, 2.0)));
	coordinates3.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-5.0, -2.0)));
	coordinates3.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-4.0, -2.0)));
	sml::AngleBounds solution3;
	solution3.setLowerBound(std::numbers::pi - solution1.getUpperBound());
	solution3.setUpperBound(std::numbers::pi + (2.0 * std::numbers::pi - solution1.getLowerBound()));
	solution3.setExtent(solution1.getExtent());

	// This object wraps around the link like a mirrored c
	std::vector<sml::Coordinate2D> coordinates4;
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, 4.0)));
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, 5.0)));
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(5.0, 5.0)));
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(5.0, -3.0)));
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, -3.0)));
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(-1.0, -2.0)));
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(4.0, -2.0)));
	coordinates4.push_back(sml::GeoUtils::projectionToCoordinate(origin, sml::Vector2d(4.0, 4.0)));
	sml::AngleBounds solution4;
	solution4.setLowerBound(4.124386376837123);
	solution4.setUpperBound(2.1587989303424644);
	solution4.setExtent(4.317597860684928);
	
	// Test and check routine
	auto check =  [&](const sml::AngleBounds& solution, const std::vector<sml::Coordinate2D>& coordinates) {
		std::vector<sml::Vector2d> vertices(coordinates.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i] = sml::GeoUtils::projectToPlane(origin, coordinates[i]);
		}
		auto b = sml::computeAngleBounds(p0, p1, vertices);
		//b.print("result");
		//solution.print("solution");
		EXPECT_LT(std::abs(b.getLowerBound() - solution.getLowerBound()), 1e-6);
		EXPECT_LT(std::abs(b.getUpperBound() - solution.getUpperBound()), 1e-6);
		EXPECT_LT(std::abs(b.getExtent() - solution.getExtent()), 1e-6);
	};

	check(solution0, coordinates0);
	check(solution1, coordinates1);
	check(solution2, coordinates2);
	check(solution3, coordinates3);
	check(solution4, coordinates4);
}


TEST(sml_test, angleBounds_merge)
{
	// Two halves should merge to a whole
	sml::AngleBounds b0(0.0, std::numbers::pi, std::numbers::pi);
	sml::AngleBounds b1(std::numbers::pi, 0.0, std::numbers::pi);
	sml::AngleBounds sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 2.0 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 2.0 * std::numbers::pi), accuracy);
	// TODO mirror
	// Alternative version #1
	b0 = sml::AngleBounds(0.0, std::numbers::pi, std::numbers::pi);
	b1 = sml::AngleBounds(std::numbers::pi, 2.0 * std::numbers::pi, std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 2.0 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 2.0 * std::numbers::pi), accuracy);
	// TODO mirror
	// Alternative version #2
	b0 = sml::AngleBounds(1.0, std::numbers::pi + 1.0, std::numbers::pi);
	b1 = sml::AngleBounds(std::numbers::pi + 1.0, 1.0, std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 2.0 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 2.0 * std::numbers::pi), accuracy);
	// TODO mirror

	// Connected boundaries
	b0 = sml::AngleBounds(0.0, 0.4 * std::numbers::pi, 0.4 * std::numbers::pi);
	b1 = sml::AngleBounds(0.4 * std::numbers::pi, 0.8 * std::numbers::pi, 0.4 * std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.8 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.8 * std::numbers::pi), accuracy);
	sol = sml::mergeBounds(b1, b0);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.8 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.8 * std::numbers::pi), accuracy);
	// Alternative version #1
	b0 = sml::AngleBounds(2.0 * std::numbers::pi - 1.0, 0.4 * std::numbers::pi - 1.0, 0.4 * std::numbers::pi);
	b1 = sml::AngleBounds(0.4 * std::numbers::pi - 1.0, 0.8 * std::numbers::pi - 1.0, 0.4 * std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - (0.8 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.8 * std::numbers::pi), accuracy);
	sol = sml::mergeBounds(b1, b0);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - (0.8 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.8 * std::numbers::pi), accuracy);
	// Alternative version #2
	b0 = sml::AngleBounds(2.0 * std::numbers::pi - 0.8 * std::numbers::pi, 2.0 * std::numbers::pi - 0.4 * std::numbers::pi, 0.4 * std::numbers::pi);
	b1 = sml::AngleBounds(2.0 * std::numbers::pi - 0.4 * std::numbers::pi, 2.0 * std::numbers::pi, 0.4 * std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 0.8 * std::numbers::pi)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.8 * std::numbers::pi), accuracy);
	sol = sml::mergeBounds(b1, b0);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 0.8 * std::numbers::pi)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.8 * std::numbers::pi), accuracy);

	// Overlapping boundaries
	b0 = sml::AngleBounds(0.0, 0.4 * std::numbers::pi, 0.4 * std::numbers::pi);
	b1 = sml::AngleBounds(0.2 * std::numbers::pi, 0.6 * std::numbers::pi, 0.4 * std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.6 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.6 * std::numbers::pi), accuracy);
	sol = sml::mergeBounds(b1, b0);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.6 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.6 * std::numbers::pi), accuracy);
	// Alternative version #1
	b0 = sml::AngleBounds(2.0 * std::numbers::pi - 1.0, 0.4 * std::numbers::pi - 1.0, 0.4 * std::numbers::pi);
	b1 = sml::AngleBounds(0.0, 0.4 * std::numbers::pi, 0.4 * std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.4 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - (0.4 * std::numbers::pi + 1.0)), accuracy);
	sol = sml::mergeBounds(b1, b0);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.4 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - (0.4 * std::numbers::pi + 1.0)), accuracy);

	// Exactly the same
	b0 = sml::AngleBounds(0.0, 0.4 * std::numbers::pi, 0.4 * std::numbers::pi);
	b1 = sml::AngleBounds(0.0, 0.4 * std::numbers::pi, 0.4 * std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.4 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.4 * std::numbers::pi), accuracy);
	// Alternative version #1
	b0 = sml::AngleBounds(2.0 * std::numbers::pi - 1.0, 0.0, 1.0);
	b1 = sml::AngleBounds(2.0 * std::numbers::pi - 1.0, 0.0, 1.0);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 1.0), accuracy);
	// Alternative version #2
	b0 = sml::AngleBounds(2.0 * std::numbers::pi - 1.0, 2.0 * std::numbers::pi, 1.0);
	b1 = sml::AngleBounds(2.0 * std::numbers::pi - 1.0, 0.0, 1.0);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 1.0), accuracy);
	// Alternative version #3
	b0 = sml::AngleBounds(0.0, 1.5 * std::numbers::pi, 1.5 * std::numbers::pi);
	b1 = sml::AngleBounds(0.0, 1.5 * std::numbers::pi, 1.5 * std::numbers::pi);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - (0.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - (1.5 * std::numbers::pi)), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 1.5 * std::numbers::pi), accuracy);

	// Inside the other
	b0 = sml::AngleBounds(0.0, 0.4 * std::numbers::pi, 0.4 * std::numbers::pi);
	b1 = sml::AngleBounds(0.1, 0.2, 0.1);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.4 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.4 * std::numbers::pi), accuracy);
	sol = sml::mergeBounds(b1, b0);
	EXPECT_LT(std::abs(sol.getLowerBound() - 0.0), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 0.4 * std::numbers::pi), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 0.4 * std::numbers::pi), accuracy);
	// Alternative version #1
	b0 = sml::AngleBounds(2.0 * std::numbers::pi - 1.0, 1.0, 2.0);
	b1 = sml::AngleBounds(2.0 * std::numbers::pi - 0.5, 0.5, 1.0);
	sol = sml::mergeBounds(b0, b1);
	EXPECT_LT(std::abs(sol.getLowerBound() - (2.0 * std::numbers::pi - 1.0)), accuracy);
	EXPECT_LT(std::abs(sol.getUpperBound() - 1.0), accuracy);
	EXPECT_LT(std::abs(sol.getExtent() - 2.0), accuracy);
}



TEST(sml_test, line_intersections)
{
	sml::Line lineA({ 0.0, 0.0 }, {0.0, 1.0});
	sml::Line lineB({ 0.0, 0.0 }, { 1.0, 0.0 });
	sml::Line lineC({ 1e-8, 0.0 }, { 1.0, 0.0 });
	sml::Line lineD({ -1.0, -1.5 }, { 1.0, 0.5 });

	sml::Vector2d result;
	EXPECT_FALSE(lineA.computeIntersection(lineB, result)); // Intersection excludes vertices of the line
	EXPECT_NEAR(result[0], 0.0, accuracy);
	EXPECT_NEAR(result[1], 0.0, accuracy);
	EXPECT_FALSE(lineA.computeIntersection(lineC, result));
	EXPECT_TRUE(lineB.computeIntersection(lineD, result));
	EXPECT_NEAR(result[0], 0.5, accuracy);
	EXPECT_NEAR(result[1], 1.5 * std::sqrt(2), accuracy);
}



TEST(sml_test, projectPointOnLine)
{
	sml::Line line({ 0.0, 0.0 }, { 1.0, 0.0 });

	sml::Vector2d point0(0.1, 0.11);
	sml::Vector2d dir0(0.0, -1.0);
	sml::Vector2d dir1(0.0, 1.0);
	sml::Vector2d dir2(-std::sqrt(2.0), -std::sqrt(2.0));
	sml::Vector2d dir3(std::sqrt(2.0), -std::sqrt(2.0));
	sml::Vector2d result;

	EXPECT_FALSE(line.isPointOnLine(point0));

	// Project down line
	EXPECT_TRUE(line.projectPointOnLine(point0, dir0, result));
	EXPECT_TRUE(line.isPointOnLine(result));
	EXPECT_NEAR(result[0], 0.1, accuracy);
	EXPECT_NEAR(result[1], 0.0, accuracy);

	// Project away from line
	EXPECT_FALSE(line.projectPointOnLine(point0, dir1, result));
	EXPECT_TRUE(line.isPointOnLine(result));
	EXPECT_NEAR(result[0], 0.1, accuracy);
	EXPECT_NEAR(result[1], 0.0, accuracy);

	// Project down line under and angle, missing the start point
	EXPECT_FALSE(line.projectPointOnLine(point0, dir2, result));

	// Project down line under and angle
	EXPECT_TRUE(line.projectPointOnLine(point0, dir3, result));
	EXPECT_TRUE(line.isPointOnLine(result));
	EXPECT_NEAR(result[0], 0.21, accuracy);
	EXPECT_NEAR(result[1], 0.0, accuracy);

	sml::Line line2({ 0.0, 0.0 }, { 0.0, -1.0 });
	sml::Vector2d point1(0.1, -0.5);
	sml::Vector2d dir4(-1.0, 0.0);

	EXPECT_TRUE(line2.projectPointOnLine(point1, dir4, result));
	EXPECT_TRUE(line2.isPointOnLine(result));
	EXPECT_NEAR(result[0], 0.0, accuracy);
	EXPECT_NEAR(result[1], -0.5, accuracy);
}



TEST(sml_test, distanceToLine)
{
	sml::Line line({ 0.0, 1.0 }, { 1.0, 1.0 });

	EXPECT_NEAR(line.distanceToPoint({0.0, 1.0}), 0.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ 1.0, 1.0 }), 0.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ 0.0, 0.0 }), 1.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ 1.0, 0.0 }), 1.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ 0.0, 2.0 }), 1.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ 1.0, 2.0 }), 1.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ 0.5, 2.0 }), 1.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ -1.0, 1.0 }), 1.0, accuracy);
	EXPECT_NEAR(line.distanceToPoint({ 2.0, 1.0 }), 1.0, accuracy);

}



TEST(sml_test, polygon_isInside)
{
	sml::Polygon polygon;
	polygon.addVertex(sml::Vector2d(- 1.0, -1.0));
	polygon.addVertex(sml::Vector2d(-1.0, 1.0));
	polygon.addVertex(sml::Vector2d(2.0, 1.0));
	polygon.addVertex(sml::Vector2d(2.0, -1.0));

	EXPECT_TRUE(polygon.isInside(sml::Vector2d(0.0, 0.0)));
	EXPECT_TRUE(polygon.isInside(sml::Vector2d(0.8, 0.55)));
	EXPECT_TRUE(polygon.isInside(sml::Vector2d(0.8, 0.55)));
	EXPECT_TRUE(polygon.isInside(sml::Vector2d(-0.8, -0.55)));
	EXPECT_FALSE(polygon.isInside(sml::Vector2d(3.0, 0.0)));
	EXPECT_FALSE(polygon.isInside(sml::Vector2d(-3.0, 0.0)));


	// Triangle, shooting a ray through the bottom vertex
	sml::Polygon polygon2;
	polygon2.addVertex(sml::Vector2d(0.0, 0.0));
	polygon2.addVertex(sml::Vector2d(-1.0, 1.0));
	polygon2.addVertex(sml::Vector2d(1.0, 1.0));
	EXPECT_FALSE(polygon2.isInside(sml::Vector2d(-1.0, 0.0)));			// Evaluated right of bottom vertex
	EXPECT_FALSE(polygon2.isInside(sml::Vector2d(0.0, 0.0)));			// Evaluated at bottom vertex
	EXPECT_FALSE(polygon2.isInside(sml::Vector2d(-2.0, 1.0)));			// Evaluated left of top edge
	EXPECT_FALSE(polygon2.isInside(sml::Vector2d(0.0, 1.0)));			// Evaluated in the middle of the top edge
}



// TODO concave



TEST(sml_test, polygon_area)
{
	// Simple convex polygon (rectangle
	sml::Polygon polygon0;
	polygon0.addVertex(sml::Vector2d(0.0, 0.0));
	polygon0.addVertex(sml::Vector2d(0.0, 2.0));
	polygon0.addVertex(sml::Vector2d(2.0, 2.0));
	polygon0.addVertex(sml::Vector2d(2.0, 0.0));
	EXPECT_NEAR(polygon0.getArea(), 4.0, accuracy);

	// Concave polygon
	sml::Polygon polygon1;
	polygon1.addVertex(sml::Vector2d(0.0, 0.0));
	polygon1.addVertex(sml::Vector2d(1.5, 1.0));
	polygon1.addVertex(sml::Vector2d(0.0, 2.0));
	polygon1.addVertex(sml::Vector2d(2.0, 2.0));
	polygon1.addVertex(sml::Vector2d(2.0, 0.0));
	EXPECT_NEAR(polygon1.getArea(), 2.5, accuracy);

	// Hex
	sml::Polygon polygon2;
	double r = 2.0;
	for (size_t i = 0; i < 6; i++) {
		double theta = 2 * std::numbers::pi / 6 * i;
		polygon2.addVertex(sml::Vector2d(r * std::cos(theta), r * std::sin(theta)));
	}
	EXPECT_NEAR(polygon2.getArea(),3 * sqrt(3) / 2.0 * r * r, accuracy);

}



TEST(sml_test, polygon_areaAndCentroid)
{
	sml::Polygon largeRectangle;
	largeRectangle.addVertex({ 10.0, 3.0});
	largeRectangle.addVertex({ 13.0, 3.0 });
	largeRectangle.addVertex({ 13.0, 5.0 });
	largeRectangle.addVertex({ 10.0, 5.0 });
	auto [centroidL, areaL] = largeRectangle.getCentroidAndArea();
	EXPECT_NEAR(areaL, 6.0, accuracy);
	EXPECT_NEAR(centroidL[0], 11.5, accuracy);
	EXPECT_NEAR(centroidL[1], 4.0, accuracy);


	// Small rectangle
	sml::Polygon smallRectangle;
	smallRectangle.addVertex({ 11.0, 4.0 });
	smallRectangle.addVertex({ 12.0, 4.0 });
	smallRectangle.addVertex({ 12.0, 5.0 });
	smallRectangle.addVertex({ 11.0, 5.0 });
	auto [centroidS, areaS] = smallRectangle.getCentroidAndArea();
	EXPECT_NEAR(areaS, 1.0, accuracy);
	EXPECT_NEAR(centroidS[0], 11.5, accuracy);
	EXPECT_NEAR(centroidS[1], 4.5, accuracy);

	// large - small
	sml::Polygon pConcave;
	pConcave.addVertex({ 10.0, 3.0});
	pConcave.addVertex({ 13.0, 3.0});
	pConcave.addVertex({ 13.0, 5.0});
	pConcave.addVertex({ 12.0, 5.0});
	pConcave.addVertex({ 12.0, 4.0});
	pConcave.addVertex({ 11.0, 4.0});
	pConcave.addVertex({ 11.0, 5.0});
	pConcave.addVertex({ 10.0, 5.0});
	auto [centroidC, areaC] = pConcave.getCentroidAndArea();
	EXPECT_NEAR(areaC, 5.0, accuracy);
	EXPECT_NEAR(centroidC[0], 11.5, accuracy);
	EXPECT_NEAR(centroidC[1], (4.0 * 6.0 - 4.5) / 5.0, accuracy);
}



TEST(sml_test, polygon_intersection_simple)
{
	// Simple case first of two overlapping squares
	sml::Polygon p0;
	p0.addVertex({ 0.0, 0.0 });
	p0.addVertex({ 1.0, 0.0 });
	p0.addVertex({ 1.0, 1.0 });
	p0.addVertex({ 0.0, 1.0 });

	// Overlapping top right side
	sml::Polygon p1;
	p1.addVertex({ 0.5, 0.5 });
	p1.addVertex({ 1.5, 0.5 });
	p1.addVertex({ 1.5, 1.5 });
	p1.addVertex({ 0.5, 1.5 });

	std::vector<sml::Polygon> result0 = p0.getIntersectionWith(p1);
	EXPECT_TRUE(result0[0].getVertexCount() == 4);
	EXPECT_NEAR(result0[0].getArea(), 0.25, accuracy);

	// Overlapping on left top side
	sml::Polygon p2;
	p2.addVertex({ -0.5, 0.5 });
	p2.addVertex({ 0.5, 0.5 });
	p2.addVertex({ 0.5, 1.5 });
	p2.addVertex({ -0.5, 1.5 });

	std::vector<sml::Polygon> result1 = p0.getIntersectionWith(p2);
	EXPECT_TRUE(result1[0].getVertexCount() == 4);
	EXPECT_NEAR(result1[0].getArea(), 0.25, accuracy);

	// Partially overlapping vertices
	sml::Polygon p3;
	p3.addVertex({ 0.0, 0.0 });
	p3.addVertex({ 2.0, 0.0 });
	p3.addVertex({ 2.0, 2.0 });
	p3.addVertex({ 0.0, 2.0 });
	std::vector<sml::Polygon> result2 = p0.getIntersectionWith(p3);
	EXPECT_TRUE(result2[0].getVertexCount() == 4);
	EXPECT_NEAR(result2[0].getArea(), 1.0, accuracy);

	// Polygon lies inside the other
	sml::Polygon p4;
	p4.addVertex({ 0.01, 0.01 });
	p4.addVertex({ 0.99, 0.01 });
	p4.addVertex({ 0.99, 0.99 });
	p4.addVertex({ 0.01, 0.99 });
	std::vector<sml::Polygon> result3 = p0.getIntersectionWith(p4);
	EXPECT_TRUE(result3[0].getVertexCount() == 4);
	EXPECT_NEAR(result3[0].getArea(), 0.98 * 0.98, accuracy);

	// 
	sml::Polygon p5;
	p5.addVertex({ 3425.2150520760802, -555.97463322228805 });
	p5.addVertex({ 3425.2150520760802, 555.97463322307817 });
	p5.addVertex({ -3425.2150520760201, 555.97463322307817 });
	p5.addVertex({ -3425.2150520760201, -555.97463322228805 });

	sml::Polygon p6;
	p6.addVertex({ 1913.1146661954224, 444.19884232060639 });
	p6.addVertex({ 1913.1289420175801, 444.24676101113232 });
	p6.addVertex({ 1410.1508623641535, 594.09279260325354 });
	p6.addVertex({ 1410.1365865419957, 594.04487391272767 });

	std::vector<sml::Polygon> result4 = p5.getIntersectionWith(p6);
	EXPECT_TRUE(result4[0].getVertexCount() == 4);


	sml::Polygon p7;
	p7.addVertex({ -1986.9138585086962, 2008.2015851706865 });
	p7.addVertex({ -2003.2821471438283, -1990.1519818738780 });
	p7.addVertex({ 1983.6163444576043, -2008.2015851698961 });
	p7.addVertex({ 2003.2821471438283, 1990.1285762467055 });

	sml::Polygon p8;
	p8.addVertex({ -57.823796735870467, 0.66795052970898849 });
	p8.addVertex({ -52.123854914862392, 0.64219713919093269 });
	p8.addVertex({ -6.9610755434943457, 9996.4217707584485 });
	p8.addVertex({ -12.661017364502420, 9996.4475241489654 });
	std::vector<sml::Polygon> result5 = p7.getIntersectionWith(p8);
	EXPECT_TRUE(result5[0].getVertexCount() == 4);
}



TEST(sml_test, polygon_intersection_double)
{
	sml::Polygon p0;
	p0.addVertex({ 0.0, 0.0 });
	p0.addVertex({1.0, 0.0});
	p0.addVertex({ 1.0, 0.2 });
	p0.addVertex({0.3, 0.2});
	p0.addVertex({ 0.3, 0.8 });
	p0.addVertex({ 1.0, 0.8 });
	p0.addVertex({ 1.0,1.0 });
	p0.addVertex({ 0.0, 1.0 });

	sml::Polygon p1;
	p1.addVertex({0.5, -1.0});
	p1.addVertex({ 1.5, -1.0 });
	p1.addVertex({1.5, 2.0});
	p1.addVertex({0.5, 2.0});

	std::vector<sml::Polygon> result = p0.getIntersectionWith(p1);
	EXPECT_TRUE(result.size() == 2);
	EXPECT_TRUE(result[0].getVertexCount() == 4);
	EXPECT_TRUE(result[1].getVertexCount() == 4);
	EXPECT_NEAR(result[0].getArea(), 0.2 * 0.5, accuracy);
	EXPECT_NEAR(result[1].getArea(), 0.2 * 0.5, accuracy);
}



TEST(sml_test, polygon_bouding_box)
{
	sml::Polygon p0;
	p0.addVertex({-0.3, 0.0});
	p0.addVertex({ 0.0,-0.5 });
	p0.addVertex({ 0.0, 0.0 });
	p0.addVertex({ 0.7, 0.1 });
	p0.addVertex({ 0.1, 0.6 });

	auto [xMin, xMax, yMin, yMax] = p0.getBoundingBox();
	EXPECT_TRUE(xMin == -0.3);
	EXPECT_TRUE(xMax == 0.7);
	EXPECT_TRUE(yMin = -0.5);
	EXPECT_TRUE(yMax = 0.6);
}



TEST(sml_test, angular_distribution_function)
{
	// Woops, defined a value for 2pi 
	std::vector<double> angles0 = { 0.0, 0.1, 0.4, 2.0 * std::numbers::pi };
	std::vector<double> values0 = { 1.0, 1.0, 1.0, 1.0 };
	sml::AngularDistributionFunction adf0(angles0, values0, true);
	EXPECT_FALSE(adf0.isValid());

	// or larger
	std::vector<double> angles1 = { 0.1, 0.1, 0.4, 7.0 };
	std::vector<double> values1 = { 1.0, 1.0, 1.0, 1.0 };
	sml::AngularDistributionFunction adf1(angles1, values1, true);
	EXPECT_FALSE(adf0.isValid());

	// Woops, forgot to define 0.0
	std::vector<double> angles2 = { 0.1, 0.1, 0.4, 7.0 };
	std::vector<double> values2 = { 1.0, 1.0, 1.0, 1.0 };
	sml::AngularDistributionFunction adf2(angles2, values2, true);
	EXPECT_FALSE(adf2.isValid());

	// Correct definition
	std::vector<double> angles3 = { 0.0, 0.1, 0.4, 5.0 };
	std::vector<double> values3 = { 1.0, 1.0, 1.0, 1.0 };
	sml::AngularDistributionFunction adf3(angles3, values3, true);
	EXPECT_TRUE(adf3.isValid());

	// And now with the right scaling
	std::vector<double> angles4 = { 0.0 };
	std::vector<double> values4 = { 1.0 / (2.0 * std::numbers::pi) };
	sml::AngularDistributionFunction adf4(angles4, values4, true);
	EXPECT_TRUE(adf4.isValid());
	double result = adf4.integrate(0.0, 2.0 * std::numbers::pi);
	EXPECT_NEAR(result, 1.0, accuracy);
	result = adf4.integrate(std::numbers::pi, std::numbers::pi);
	EXPECT_NEAR(result, 0.0, accuracy);
	result = adf4.integrate(std::numbers::pi + 1e-8, std::numbers::pi - 1e-8);
	EXPECT_NEAR(result, 1.0, accuracy);

	std::vector<double> angles5 = { 0.0, 0.15, 0.4, 6.2 };
	std::vector<double> values5 = { 1.0, 0.0, 0.0, 0.0 };
	sml::AngularDistributionFunction adf5(angles5, values5, true);
	EXPECT_TRUE(adf5.isValid());
	result = adf4.integrate(0.0, 2.0 * std::numbers::pi);
	EXPECT_NEAR(result, 1.0, accuracy);
	result = adf4.integrate(std::numbers::pi + 1e-8, std::numbers::pi - 1e-8);
	EXPECT_NEAR(result, 1.0, accuracy);


	// Iteratively integration
	std::vector<double> angles6 = { 0.0, 0.15, 0.4, 6.2 };
	std::vector<double> values6 = { 1.0, 2.0, 3.0, 5.0 };
	sml::AngularDistributionFunction adf6(angles6, values6, true);
	EXPECT_TRUE(adf6.isValid());
	result = adf6.integrate(0.0, 2.0 * std::numbers::pi);
	EXPECT_NEAR(result, 1.0, accuracy);
	double result0 = adf6.integrate(1.0, 5.0);
	double result1 = 0.0;
	size_t numBins = 200;
	double binSize = 4.0 / numBins;
	for (size_t i = 0; i < numBins; i++) {
		double lowerBound = 1.0 + binSize * i;
		double upperBound = 1.0 + binSize * (i + 1);
		result1 += adf6.integrate(lowerBound, upperBound);
	}
	EXPECT_NEAR(result0, result1, accuracy);

	// Add weights
	sml::AngularDistributionFunction adf7(angles6, values6, true);
	result = adf7.integrate(0.0, 2.0 * std::numbers::pi, [](size_t i) {return 0.5; });
	EXPECT_NEAR(result, 0.5, accuracy);
}



TEST(sml_test, segments_singleObject)
{
	sml::Line link({ 1.0, 0.0 }, { 2.0, 0.0 });

	// Test a link with a box directly above it with downwards projection
	std::vector<sml::Vector2d> vertices0;
	vertices0.push_back({1.0, 1.0});
	vertices0.push_back({ 2.0, 1.0 });
	vertices0.push_back({ 2.0, 2.0 });
	vertices0.push_back({ 1.0, 2.0 });
	vertices0.push_back({ 1.0, 1.0 });
	std::vector<sml::Segment> segments0 = sml::computeSegments(link, 0.5 * std::numbers::pi, vertices0);
	EXPECT_EQ(segments0.size(), 1);
	EXPECT_NEAR(segments0[0].line.getStart()[0], 1.0, accuracy);
	EXPECT_NEAR(segments0[0].line.getEnd()[0], 2.0, accuracy);
	EXPECT_NEAR(segments0[0].distanceStart, 1.0, accuracy);
	EXPECT_NEAR(segments0[0].distanceEnd, 1.0, accuracy);

	// Make the box a bit larger
	std::vector<sml::Vector2d> vertices1;
	vertices1.push_back({ 0.0, 1.0 });
	vertices1.push_back({ 3.0, 1.0 });
	vertices1.push_back({ 3.0, 2.0 });
	vertices1.push_back({ 0.0, 2.0 });
	vertices1.push_back({ 0.0, 1.0 });
	std::vector<sml::Segment> segments1 = sml::computeSegments(link, 0.5 * std::numbers::pi, vertices1);
	EXPECT_EQ(segments1.size(), 1);
	EXPECT_NEAR(segments1[0].line.getStart()[0], 1.0, accuracy);
	EXPECT_NEAR(segments1[0].line.getEnd()[0], 2.0, accuracy);
	EXPECT_NEAR(segments1[0].distanceStart, 1.0, accuracy);
	EXPECT_NEAR(segments1[0].distanceEnd, 1.0, accuracy);

	// Make the box a bit smaller
	std::vector<sml::Vector2d> vertices2;
	vertices2.push_back({ 1.25, 1.0 });
	vertices2.push_back({ 1.75, 1.0 });
	vertices2.push_back({ 1.75, 2.0 });
	vertices2.push_back({ 1.25, 2.0 });
	vertices2.push_back({ 1.25 , 1.0 });
	std::vector<sml::Segment> segments2 = sml::computeSegments(link, 0.5 * std::numbers::pi, vertices2);
	EXPECT_EQ(segments2.size(), 1);
	EXPECT_NEAR(segments2[0].line.getStart()[0], 1.25, accuracy);
	EXPECT_NEAR(segments2[0].line.getEnd()[0], 1.75, accuracy);
	EXPECT_NEAR(segments2[0].distanceStart, 1.0, accuracy);
	EXPECT_NEAR(segments2[0].distanceEnd, 1.0, accuracy);

	// Direction in line of link: Box infront of link
	std::vector<sml::Vector2d> vertices3;
	vertices3.push_back({ 3.0, -1.0 });
	vertices3.push_back({ 3.0, 1.0 });
	vertices3.push_back({ 4.0, 1.0 });
	vertices3.push_back({ 4.0, -1.0 });
	vertices3.push_back({ 3.0 , -1.0 });
	std::vector<sml::Segment> segments3 = sml::computeSegments(link, 0.0, vertices3);
	EXPECT_EQ(segments3.size(), 1);
	EXPECT_NEAR(segments3[0].line.getStart()[0], 1.0, accuracy);
	EXPECT_NEAR(segments3[0].line.getStart()[1], 0.0, accuracy);
	EXPECT_NEAR(segments3[0].line.getEnd()[0], 2.0, accuracy);
	EXPECT_NEAR(segments3[0].line.getEnd()[1], 0.0, accuracy);
	EXPECT_NEAR(segments3[0].distanceStart, 2.0, accuracy);
	EXPECT_NEAR(segments3[0].distanceEnd, 1.0, accuracy);


	// Direction in line of link: Box not infront of link
	std::vector<sml::Vector2d> vertices4;
	vertices4.push_back({ 3.0, 0.0 });
	vertices4.push_back({ 3.0, 1.0 });
	vertices4.push_back({ 4.0, 1.0 });
	vertices4.push_back({ 4.0, 0.0 });
	vertices4.push_back({ 3.0 , 0.0 });
	std::vector<sml::Segment> segments4 = sml::computeSegments(link, 0.0, vertices4);
	EXPECT_EQ(segments4.size(), 0);
}



TEST(sml_test, segments_multipleObjects)
{
	// TODO the test where we need an extra point...
}



TEST(sml_test, polylineclipper)
{
	sml::Line line(0.0, 10.0);
	std::vector<sml::Vector2d> polyline;
	sml::PolylineClipper clipper;
	std::vector<std::vector<sml::Vector2d>> newPolyLines;
	EXPECT_FALSE(clipper.clip(polyline, line, newPolyLines)); // Nothing to clip

	// Above line
	polyline.clear();
	polyline.push_back({ 1.0, 1.0 });
	polyline.push_back({ 9.0, 1.0 });
	polyline.push_back({ 9.0, 2.0 });
	polyline.push_back({ 1.0, 2.0 });
	polyline.push_back({ 1.0, 1.0 });
	EXPECT_FALSE(clipper.clip(polyline, line, newPolyLines)); // Nothing to clip

	// Below line
	polyline.clear();
	polyline.push_back({ 1.0, -1.0 });
	polyline.push_back({ 9.0, -1.0 });
	polyline.push_back({ 9.0, -2.0 });
	polyline.push_back({ 1.0, -2.0 });
	polyline.push_back({ 1.0, -1.0 });
	EXPECT_FALSE(clipper.clip(polyline, line, newPolyLines)); // Nothing to clip

	// Infront of line
	polyline.clear();
	polyline.push_back({ 11.0, -1.0 });
	polyline.push_back({ 12.0, -1.0 });
	polyline.push_back({ 12.0, 1.0 });
	polyline.push_back({ 11.0, 1.0 });
	polyline.push_back({ 11.0, -1.0 });
	EXPECT_FALSE(clipper.clip(polyline, line, newPolyLines)); // Nothing to clip

	// Behind line
	polyline.clear();
	polyline.push_back({ -11.0, -1.0 });
	polyline.push_back({ -12.0, -1.0 });
	polyline.push_back({ -12.0, 1.0 });
	polyline.push_back({ -11.0, 1.0 });
	polyline.push_back({ -11.0, -1.0 });
	EXPECT_FALSE(clipper.clip(polyline, line, newPolyLines)); // Nothing to clip

	// Square polyline intersecting with line
	polyline.clear();
	polyline.push_back({ 1.0, -1.0 });
	polyline.push_back({ 9.0, -1.0 });
	polyline.push_back({ 9.0, 1.0 });
	polyline.push_back({ 1.0, 1.0 });
	polyline.push_back({ 1.0, -1.0 });
	EXPECT_TRUE(clipper.clip(polyline, line, newPolyLines));
	EXPECT_TRUE(newPolyLines.size() == 3);
}



int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(print_time) = true;
	::testing::GTEST_FLAG(output) = "xml:results.xml";
	return RUN_ALL_TESTS();
}

