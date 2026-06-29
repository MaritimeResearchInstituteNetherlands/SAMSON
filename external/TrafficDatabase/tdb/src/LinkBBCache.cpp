// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include "util/LinkBBCache.h"


namespace tdb {

//LinkBBCache* LinkBBCache::m_instance = nullptr;
//
//
//sml::Polygon& LinkBBCache::getBB(const Data<LinkScheme>& link, const IndexedDataTable<WaypointPoint, WaypointPoint::UidType>& indexedAreaWp)
//{
//	if (m_bbCache.count(link->getUID()) <= 0) {
//		m_bbCache.emplace(std::make_pair(link->getUID(),
//			sml::Polygon{ indexedAreaWp.get(link->getStart())->getPosition(), indexedAreaWp.get(link->getEnd())->getPosition(), 100 }));
//	}
//	return  m_bbCache[link->getUID()];
//}
} // namespace tdb
