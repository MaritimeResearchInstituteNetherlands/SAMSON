// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sml/Functions.h>
#include <numbers>
#include <cmath>

namespace sml {

double cdf_normalDistribution(const double x, const double mu, const double sigma)
{
	using namespace std::numbers;
	double z = (mu - x) / (sigma * sqrt2);
	return 0.5 * std::erfc(z);
}



// Normalises the angle to [0, 2 * pi]
double normalisedPositiveAngles(double angle)
{
	double twopi = 2.0 * std::numbers::pi;
	double invtwopi = 1.0 / (2.0 * std::numbers::pi);
	return angle - std::floor(angle * invtwopi) * twopi;
}


// Normalises the angle to [-pi, pi]
double normalisedBalancedAngles(double angle)
{
	double twopi = 2.0 * std::numbers::pi;
	double invtwopi = 1.0 / (2.0 * std::numbers::pi);
	return angle - round(angle * invtwopi) * twopi;
}

} // namespace sml

