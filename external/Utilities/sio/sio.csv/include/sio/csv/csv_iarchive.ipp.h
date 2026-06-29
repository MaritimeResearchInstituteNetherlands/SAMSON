// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sio/csv/csv_iarchive.h>

#include <string>
#include <sstream>
#include <list>

namespace {
	std::list<std::string> split(const std::string& str, const std::string& delim)
	{
		std::list<std::string> result;
		size_t start = 0;
		for (size_t found = str.find(delim); found != std::string::npos; found = str.find(delim, start))
		{
			result.emplace_back(str.begin() + start, str.begin() + found);
			start = found + delim.size();
		}
		if (start != str.size())
			result.emplace_back(str.begin() + start, str.end());
		return result;
	}
}

namespace boost {
	namespace archive {

		template<class T>
		constexpr void csv_iarchive::load_override(T& t) {
			load_start(typeid(T).name());	
			this->base::load_override(t);			
			load_end(typeid(T).name());
		}

		/*
		template<class T>
		void csv_iarchive::load_override(std::vector<T>& t) {			
			while (_is.peek() != '\n' && _is.peek() != EOF) {
				std::cout << "_is.peek: " << _is.peek() << "\n";
				T temp;
				load_start(typeid(T).name());
				this->load_override(temp);
				load_end(typeid(T).name());
				t.emplace_back(temp);
			}
		}
		*/

		/*
		template<class T>
		void load_override(boost::serialization::nvp<std::vector<T>>& t) 
		*/
		template<class T>
		void csv_iarchive::load_override(std::vector<T>& t) 
		{
			size_t sz, version;
			this->load(sz);
			for (size_t i = 0; i < sz; ++i) {
				T temp;
				this->load_override(temp);
				t.emplace_back(temp);
			}		
		}


		//REMARK: DataTable reads are ending up here, because DataTable is simply vector::shared_ptr
		template<class T>
		void csv_iarchive::load_override(std::vector<std::shared_ptr<T>>& t) {
			while (_is.peek() != '\n' && _is.peek() != EOF) {
				auto temp = std::make_shared<T>(); //TBD: 
				load_start(typeid(T).name());
				this->load_override(*(temp.get()));
				load_end(typeid(T).name());
				t.emplace_back(temp);
				_isLineRead = true;
			}
		}

		template<class T>
		void csv_iarchive::load(T& t) {
			//time to read a line?
			if (_isLineRead)
			{
				_isLineRead = false;
				std::string str;
				std::getline(_is, str);
				std::string d(1, _delim);
				_tokens = ::split(str, d);
			}
			if (not _tokens.empty()) {
				std::stringstream strs(_tokens.front());
				strs >> t;
				_tokens.pop_front();
			}
		}
	}// namespace archive
}// namespace boost
