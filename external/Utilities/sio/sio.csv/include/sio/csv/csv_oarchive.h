// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <vector>
#include <utility>
#include <iostream>
#include <typeindex>
#include <sstream>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
	namespace archive {

		enum class csv_flags {
			newLine
		};		


		// Catches the nvp names for automatic header generation
		class HeaderGenerator {
		public:
			template <typename T>
			HeaderGenerator& operator&(const boost::serialization::nvp<T>& nvp) {
				m_names.push_back(nvp.name());
				return *this;
			}

			template <typename T>
			HeaderGenerator& operator&(const boost::serialization::nvp<std::vector<T>>& nvp) {
				m_names.push_back(std::string("num_") + std::string(nvp.name()));
				m_names.push_back(std::string(nvp.name()) + std::string(",..."));
				return *this;
			}

			template <class T>
			HeaderGenerator& operator&(T& baseObject) {
				baseObject.serialize(*this, 0);
				return *this;
			}

			std::string getHeader()
			{
				std::stringstream header;
				for (size_t i = 0; i < m_names.size(); i++) {
					if (i == 0) {
						header << m_names[i];
					}
					else {
						header << "," << m_names[i];
					}
				}
				return header.str();
			}
		private:
			std::vector<std::string> m_names;
		};



		class csv_oarchive : public detail::common_oarchive<csv_oarchive>
		{
		public:
			csv_oarchive(std::ostream& os, std::string delim = ",", unsigned int flags = 0) :
				_os(os),
				_flags(flags),
				_delim(delim),
				_currentDelim("")
			{
			}

			~csv_oarchive() BOOST_OVERRIDE 
			{
			}

			template <class T>
			std::string generateHeader(T t)
			{
				HeaderGenerator headerGenerator;
				t.serialize(headerGenerator, 0);
				return headerGenerator.getHeader();
			}

		protected:
			typedef detail::common_oarchive<csv_oarchive> base;

			// give serialization implementation access to this class
			friend class detail::interface_oarchive<csv_oarchive>;
			friend class save_access;

			//Anything not a value should be trapped here
			template<typename T>
			void save_override(T& t);

			// main overload for name-value pairs. Internally, serialization
			// is dispatched to the `common_oarchive` base, which in turn dispatches
			// the task to the `save` overloads, defined further below.		
			template<typename T>
			void save_override(boost::serialization::nvp<T> const& t);

			template<typename T>
			void save_override(boost::serialization::nvp<std::vector<T>> const& t);

			template<typename T>
			void save_override(boost::serialization::nvp<const std::vector<T>&> const& t);

			template <typename T>
			void save_override(boost::serialization::nvp<std::vector<std::shared_ptr<T>>> const& t);

			template <typename T>
			void save_override(boost::serialization::nvp<std::vector<std::unique_ptr<T>>> const& t);
				
			void save_override(const  csv_flags& t) {				
				if (t == csv_flags::newLine) {
					_os << "\n";
					_currentDelim = "";
				}
			}

			// specific overrides for attributes - not name value pairs so we
			// want to trap them before the above "fall through"
			// TBD: should be used to create meta data
			void save_override(const class_id_type& t) { }
			void save_override(const class_id_optional_type& t) {}
			void save_override(const class_id_reference_type& t) {}
			void save_override(const object_id_type& t) { }
			void save_override(const object_reference_type& t) {}
			void save_override(const version_type& t) {}
			void save_override(const class_name_type& t) { }
			void save_override(const tracking_type& t) { }

			//Everything ends up here		  
			template<typename T>
			void save(T const& t) {
				_os << _currentDelim << t;
			}			
			
		private:
			std::ostream& _os;
			unsigned int _flags;
			bool _header;
			std::string _delim;
			std::string _currentDelim;
			std::list<std::string> _stack;	
		}; //class csv_oarchive

		
		//static const  csv_flag_wrapper::flags myNewLine_csv{ csv_flag_wrapper::flags::newLine };
		static const  csv_flags myNewLine_csv{ csv_flags::newLine };
		

	} // namespace archive
} // namespace boost

#define CSV_OARCHIVE_NL() boost::serialization::make_nvp("NEWLINE", boost::archive::myNewLine_csv);

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::csv_oarchive)

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas
#include <sio/csv/csv_oarchive.ipp.h>
