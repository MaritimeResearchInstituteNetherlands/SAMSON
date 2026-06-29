// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file Types.h
 * @brief This file contains the definition of the types used in the Traffic Database.
 */
#pragma once


#include "util/IIdentifiable.h"

#include <utility>
#include <string>
#include <string_view>


namespace tdb {

	using Area_UidType = std::string;

	/**
	 * @struct Index
	 * @brief This struct is used to define indices on a 2D space or grid.
	 *
	 * It inherits from  IIdentifiable and provides a unique identifier (UID) based on its coordinates.
	 */
	struct Index : public IIdentifiable {
		Index(unsigned short x, unsigned short y): x { x }, y{ y } {}
		const std::string getUID() const override { return std::to_string(x) + " " + std::to_string(y); }
		unsigned short x;
		unsigned short y;
	};
	
} //namespace tdb
