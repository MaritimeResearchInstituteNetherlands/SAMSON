// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
#pragma once

#include <sml_export.h>
#include <sml/Vector.h>
#include <sml/Line.h>
#include <vector>

namespace sml 
{

class SML_EXPORT PolylineClipper
{
	using Vertices = std::vector<sml::Vector2d>;
public:
	bool clip(const Vertices& polyLine, const sml::Line& line, std::vector<Vertices>& newPolyLines);
};

} // namespace sml
