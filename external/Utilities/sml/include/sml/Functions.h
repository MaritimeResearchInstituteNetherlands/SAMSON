// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once
#include <sml_export.h>

namespace sml {

double SML_EXPORT cdf_normalDistribution(const double x, const double mu, const double sigma);

double SML_EXPORT normalisedPositiveAngles(double angle);

double SML_EXPORT normalisedBalancedAngles(double angle);

} // namespace sml
