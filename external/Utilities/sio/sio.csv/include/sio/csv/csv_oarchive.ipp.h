// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sio/csv/csv_oarchive.h>

namespace boost {
namespace archive {

	/*
	template<typename T>
	void csv_oarchive::save_override(boost::serialization::nvp<T> const& t) {
		std::cout << "csv_oarchive::save_override\n":
	}
	*/
    template<typename T>
	void csv_oarchive::save_override(T& t)
	{
		// TBD : forcing name-value pairs could be good 
		//    BOOST_MPL_ASSERT((serialization::is_wrapper<T>));		
		base::save_override(t);
	}

	template<typename T>
	void csv_oarchive::save_override(boost::serialization::nvp<T> const& t) {		
		_stack.push_back(t.name());
		this->save_override(t.const_value());
		_currentDelim = ",";
		_stack.pop_back();  		
		if (_stack.empty()) {
			_os << "\n";  
			_currentDelim = "";
		}		
	}
		
    
	template<typename T>
	void csv_oarchive::save_override(boost::serialization::nvp<std::vector<T>> const& t)
	{
		_stack.push_back(t.name());
		for (auto& i : t.const_value()) {
			this->save_override(i);			
			_currentDelim = ",";
			//Only the main vector adds new line
			if (_stack.size() == 1) {
				_os << "\n";
				_currentDelim = "";
			}			
		}
		_stack.pop_back();
	}



	template<typename T>
	void csv_oarchive::save_override(boost::serialization::nvp<const std::vector<T>&> const& t)
	{
		_stack.push_back(t.name());
		for (auto& i : t.const_value()) {
			this->save_override(i);
			_currentDelim = ",";
			//Only the main vector adds new line
			if (_stack.size() == 1) {
				_os << "\n";
				_currentDelim = "";
			}
		}
		_stack.pop_back();
	}



	template <typename T>
    void csv_oarchive::save_override(boost::serialization::nvp<std::vector<std::shared_ptr<T>>> const& t)
	{
		_stack.push_back(t.name());
		for (auto& i : t.const_value()) {
			this->save_override(*i);
			_currentDelim = ",";
			//Only the main vector adds new line
			if (_stack.size() == 1) {
				_os << "\n";
				_currentDelim = "";
			}
		}
		_stack.pop_back();
    }



	template <typename T>
	void csv_oarchive::save_override(boost::serialization::nvp<std::vector<std::unique_ptr<T>>> const& t)
	{
		_stack.push_back(t.name());
		for (auto& i : t.const_value()) {
			this->save_override(*i);
			_currentDelim = ",";
			//Only the main vector adds new line
			if (_stack.size() == 1) {
				_os << "\n";
				_currentDelim = "";
			}
		}
		_stack.pop_back();
	}

}// namespace archive
}// namespace boost
