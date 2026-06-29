// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once
#include <tdb.shapelib_export.h>
#include <tdb/AreaModel.h>
#include <sio/shapelib/shapelibWriter.h>
#include <filesystem>

namespace tdb {
namespace shapelib {

class TDB_SHAPELIB_EXPORT Writer
{
public:
	Writer(std::filesystem::path path, const AreaModel& areaModel);
};
} // namespace shapelib
} // namespace tdb
