// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file IIdentifiable.h
 * @brief This file contains the interface for objects that have a unique identifier (UID).
 * 
 */

#pragma once

#include <string>

namespace tdb {

	/**
	 *  @class IIdentifiable
	 * 	@brief Interface for objects that have a unique identifier (UID).
	 * 
	 *  This interface is used to provide a common way to get the unique identifier of an object.
	 */
	class IIdentifiable {
	public:
		virtual ~IIdentifiable() {}
		virtual const std::string getUID() const = 0;
	};

} //namespace tdb
