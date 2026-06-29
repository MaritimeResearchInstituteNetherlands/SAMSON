// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once


#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/archive/text_oarchive.hpp>



#include <utility>
#include <iostream>
#include <typeindex>
#include <sstream>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
	namespace archive {

		enum class csv_i_flags {
			newLine,
			endOfFile,
		};

		class csv_iarchive :
			public detail::common_iarchive<csv_iarchive>
		{
		public:
			csv_iarchive(std::istream& is, char delim = ',', unsigned int flags = 0) :
				_is(is),
				_flags(flags),
				_delim(delim),
				_stackI(0),
				_isLineRead(true) {
			}

			~csv_iarchive() BOOST_OVERRIDE {
			}

		public:
			typedef detail::common_iarchive<csv_iarchive> base;
			// give serialization implementation access to this class
			friend class detail::interface_iarchive<csv_iarchive>;


			void load_start(char const* name) {
				++_stackI;
				if (_stackI == 1)
				{
					_isLineRead = true;
				}
			};
			void load_end(char const* name) {
				--_stackI;
			};

			template<class T>
			constexpr void load_override(T& t);

			
			template<class T>
			void load_override(std::vector<T>& t);
			

			//TBD: convert into template flag
			void load_override(csv_i_flags& t) {
				//it is time to read new line
				this->_isLineRead = true;			
				if (_is.peek() == EOF) {
					t = csv_i_flags::endOfFile;
				} else {
					t = csv_i_flags::newLine;
				}
			}

		/*
			template<class T>			
			void load_override(boost::serialization::nvp<std::vector<T>>& t);
			*/


			template<class T>
			void load_override(std::vector<std::shared_ptr<T>>& t);

			//Everything ends up here		  
			template<class T>
			void load(T& t);						

			//TBD: use followings to create metadata
			void load_override(class_id_optional_type& t) { };
			void load_override(class_name_type& t) { };
			void load(class_id_optional_type& t) { };
			void load(class_id_reference_type& t) { };
			void load(object_id_type& t) { };
			void load(object_reference_type& t) { };
			void load(version_type& t) { };
			void load(tracking_type& t) { };

		public:
			std::istream& _is;
			unsigned int _flags;
			char _delim;
			std::list<std::string> _tokens;
			int _stackI;
			bool _isLineRead;
		}; //class csv_iarchive

	} // namespace archive
} // namespace boost

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::csv_iarchive)

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas
#include <sio/csv/csv_iarchive.ipp.h>
