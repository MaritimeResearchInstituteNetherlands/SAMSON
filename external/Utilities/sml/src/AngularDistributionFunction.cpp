// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sml/AngularDistributionFunction.h>
#include <algorithm>
#include <numbers>
#include <iostream>
namespace sml
{
AngularDistributionFunction::AngularDistributionFunction()
	: m_valid(false)
{
}



AngularDistributionFunction::AngularDistributionFunction(std::vector<double> angles, std::vector<double> probabilities, bool normalise)
	: m_valid(true)
{
	// Check input
	if (angles.size() != probabilities.size()) {
		LogError("AngularDistributionFunction: incorrect number of angles and values");
	}

	// Store the data
	for (size_t i = 0; i < angles.size(); i++) {
		m_data.push_back(Data(angles[i], probabilities[i]));
	}

	// Sort based on angle
	std::sort(m_data.begin(), m_data.end(), [&](const Data& a, const Data& b) {
		return a.angle < b.angle;
		}
	);
	
	if (m_data.size() == 0.0) {
		LogError("AngularDistributionFunction: empty data set.");
		m_valid = false;
	}

	// Make sure angles are within [0,2 pi)
	for (size_t i = 0; i < m_data.size(); i++) {
		if (m_data[i].angle < 0.0 || m_data[i].angle >= std::numbers::pi * 2.0) {
			LogError("AngularDistributionFunction: Invalid angle found in the angular distribution: " << m_data.back().angle);
			m_valid = false;
		}
	}

	// Ensure that 0.0 is always defined
	if (m_data[0].angle != 0.0) {
		LogError("AngularDistributionFunction: No value specified for angle 0.0");
		m_valid = false;
	}

	// Integrate and resize, throw a warning if the area is not zero
	integrateBins();
	double totalArea = 0.0;
	for (double area_i : m_area) {
		totalArea += area_i;
	}
	if (normalise) {
		if (std::abs(totalArea - 1.0) > 1e-6) {
			if (totalArea != 0.0) {
				double scaling = 1.0 / totalArea;
				for (auto& data_i : m_data) {
					data_i.probabilityDensity *= scaling;
				}
				//LogWarning << "AngularDistributionFunction: Area under distribution is not 1.0. Normalising with factor " << scaling;
				integrateBins();

				// Temp check
				double newArea = 0.0;
				for (double area_i : m_area) {
					newArea += area_i;
				}
			}
			else {
				LogError("Total area of AngularDistributionFunction is zero");
			}
		}
	}
}



double AngularDistributionFunction::getProbabilityDensity(const double angle) const
{
	if (!m_valid) {
		LogWarning("Evaluating an invalid AngularDistributionFunction");
	}

	return m_data[getBinIndex(angle)].probabilityDensity; // angle is in the last bin
}



void AngularDistributionFunction::integrateBins()
{
	// Integrate each bin
	m_area.resize(m_data.size());
	for (size_t i = 0; i < m_data.size(); i++) {
		double upperAngle = 2.0 * std::numbers::pi;
		if (i < m_data.size() - 1) {
			upperAngle = m_data[i + 1].angle;
		}
		double binSize = upperAngle - m_data[i].angle;
		m_area[i] = m_data[i].probabilityDensity * binSize;
	}
}



size_t AngularDistributionFunction::getBinIndex(const double angle) const
{
	assert(angle <= 2.0 * std::numbers::pi);

	auto lower = std::lower_bound(m_data.begin(), m_data.end(), angle, [](const Data& a, const double angle) {return a.angle <= angle;});
	return std::distance(m_data.begin(), lower) - 1;

}



double AngularDistributionFunction::getTotalArea() const
{
	double totalArea = 0.0;
	for (double area : m_area) {
		totalArea += area;
	}
	return totalArea;
}

} // namespace sml
