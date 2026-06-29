// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once
#include <sml_export.h>
#include <sio/logger/api.h>
#include <vector>
#include <cassert>
#include <functional>
#include <numbers>
#include <iostream>

namespace sml
{

// Linear angular distribution function, the bins have the values of the left angle of the bin
class SML_EXPORT AngularDistributionFunction
{
public:
	struct Data
	{
		double angle;
		double probabilityDensity;
	};
	
	AngularDistributionFunction();

	AngularDistributionFunction(std::vector<double> angles, std::vector<double> probabilities, bool normalise);

	double getProbabilityDensity(const double angle) const;

	template <class F>
	double integrate(const double lowerAngle, const double upperAngle, F weights) const;

	inline double integrate(const double lowerAngle, const double upperAngle) const { return integrate(lowerAngle, upperAngle, [](size_t) { return 1.0; }); }

	double getTotalArea() const;

	bool isValid() const { return m_valid; };

	inline const std::vector<Data>& getData() const { return m_data; }

private:
	void integrateBins();

	// lowerAngle < upperAngle and both are in [0, 2*pi]
	template <class F>
	double baseIntegration(const double angle_0, const double angle_1, F weights) const;

	size_t getBinIndex(const double angle) const;

private:
	bool m_valid;
	std::vector<Data> m_data;
	std::vector<double> m_area;
};



template <class F>
double AngularDistributionFunction::integrate(const double lowerAngle, const double upperAngle, F weights) const
{
	if (!m_valid) {
		LogWarning("Integrating an invalid AngularDistributionFunction");
	}
	if (lowerAngle == upperAngle) {
		return 0.0;
	}

	double result = 0.0;
	bool loop = lowerAngle > upperAngle;
	if (loop) {
		result += baseIntegration(lowerAngle, 2.0 * std::numbers::pi, weights);
		result += baseIntegration(0.0, upperAngle, weights);
	}
	else {
		result = baseIntegration(lowerAngle, upperAngle, weights);
	}

	return result;
}



template <class F>
double AngularDistributionFunction::baseIntegration(const double lowerAngle, const double upperAngle, F weights) const
{
	assert(lowerAngle <= 2.0 * std::numbers::pi);
	assert(upperAngle <= 2.0 * std::numbers::pi);
	// Fast bin index search
	auto lower = std::lower_bound(m_data.begin(), m_data.end(), lowerAngle, [](const Data& a, const double angle) {return a.angle <= angle;});
	auto upper = std::lower_bound(lower, m_data.end(), upperAngle, [](const Data& a, const double angle) {return a.angle <= angle;});
	size_t startBin = std::distance(m_data.begin(), lower) - 1;
	size_t endBin = startBin + std::distance(lower, upper);

	double result = 0.0;
	if (startBin == endBin) {
		double extent = upperAngle - lowerAngle;
		result = extent * m_data[startBin].probabilityDensity * weights(startBin);
	}
	else {
		// Start 
		double angle = startBin + 1 < m_data.size() ? m_data[startBin + 1].angle : 2.0 * std::numbers::pi;
		double extent = angle - lowerAngle;
		result += extent * m_data[startBin].probabilityDensity * weights(startBin);

		// End
		extent = upperAngle - m_data[endBin].angle;
		result += extent * m_data[endBin].probabilityDensity * weights(endBin);

		// Middle
		for (int i = startBin + 1; i < int(endBin); i++) {
			result += m_area[i] * weights(i);
		}
	}

	return result;
}

} // namespace sml
