// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/Object.h>

namespace sem
{
Object::Object(const size_t& index, const tdb::Data<tdb::ObjectScheme>& objectScheme, const std::vector<sml::Coordinate2D>& coordinates)
	: base(index, objectScheme, coordinates)
{
	// TODO
}



//void Object::setup()
//{
//	// TODO
//}
} // namespace sem
