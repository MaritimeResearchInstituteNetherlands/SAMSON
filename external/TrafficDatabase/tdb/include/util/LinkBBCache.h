// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file LinkBBCache.h
 * 
 * @brief This file contains the declaration of the bounding box cache of links.
 */

#pragma once

#include "tdb_export.h"
#include <tdb/AreaSchemes.h>
#include "util/IModel.h"

#include <sml/Polygon.h>


namespace tdb {	
	/**
	 * @class LinkBBCache
	 * @brief A singleton class to cache bounding boxes of links.
	 * 
	 * This class is used to cache bounding boxes of links to avoid recalculating them.
	 */
	//class TDB_EXPORT LinkBBCache{
	//public:
	//	/**
	//	 * @brief Function to get the instance of the LinkBBCache.
	//	 */
	//	static LinkBBCache& getInstance()
	//	{
	//		// If the instance doesn't exist, create it
	//		if (!m_instance) {
	//			m_instance = new LinkBBCache();
	//		}
	//		return *m_instance;
	//	}
	//
	//	/**
	//	 * @brief Function to calculate and store the bounding box of a link.
	//	 * 
	//	 * If the bounding box of the link is already calculated, it returns the cached bounding box. 
	//	 * Otherwise, it calculates the bounding box of the link and stores it in the cache.
	//	 * 
	//	 * @param[in] link The link to calculate the bounding box
	//	 * @param[in] indexedAreaWp The indexed table of area waypoints in coordinate system
	//	 * @return The bounding box of the link
	//	 */
	//	sml::Polygon& getBB(const Data<LinkScheme>& link, const  IndexedDataTable<WaypointPoint, WaypointPoint::UidType>& indexedAreaWp);
	//
	//	LinkBBCache(const LinkBBCache&) = delete;
	//	LinkBBCache& operator=(const LinkBBCache&) = delete;
	//
	//private: 				
	//	LinkBBCache() {}
	//	~LinkBBCache() {}
	//	static LinkBBCache* m_instance;
	//	std::unordered_map<LinkScheme::UidType, sml::Polygon> m_bbCache;
	//}; //class LinkBBCache


} //namespace tdb

