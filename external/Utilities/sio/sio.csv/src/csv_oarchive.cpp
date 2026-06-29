// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#define BOOST_ARCHIVE_SOURCE

#include <sio/csv/csv_oarchive.h>
#include <boost/archive/detail/archive_serializer_map.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>

namespace boost {
	namespace archive {

		template class detail::archive_serializer_map<csv_oarchive>;

	} // namespace archive
} // namespace boost

