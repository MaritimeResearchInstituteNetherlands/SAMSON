// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "util/IModel.h"
#include <sio/csv/csv_iarchive.h>
#include <sio/csv/csv_oarchive.h>
#include <fstream>
#include <unordered_map>
#include <sio/logger/api.h>

namespace tdb
{

class BaseModel
{
public:
	BaseModel() {}

	BaseModel(const BaseModel&) = delete;

	BaseModel& operator= (const BaseModel&) = delete;

	virtual bool loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames) { return true; };

	virtual void help() { LogError("Empty help implementation"); }

	template <class T>
	void loadTable(const std::string_view path, std::unordered_map<std::string, std::vector<std::string>>& filenames, const std::string& key, T& table) {
		if (!filenames.contains(key)) {
			LogWarning("Filename for " << key << " not found");
		}

		for (std::string& filename : filenames[key]) {
			readFile(path, filename, table);
		}
	}

	template <class T, class C1, class C2>
	void indexTable(const T& table, IndexedDataTable<C1, C2>& indexedTable)
	{
		indexedTable.add(table);
	}

	template<class T>
	bool readFile(std::string_view path, std::string_view f_name, T& table) {
		std::string header;
		std::string filename = std::string{ path } + std::string{ f_name };
		LogInfo("Loading: " << filename);
		std::ifstream ifs(filename);
		if (!ifs.is_open()) {
			LogWarning("failed to open " << filename);
			return false;
		}
		boost::archive::csv_iarchive ia(ifs);
		ia >> header; //throw header away	
		ia >> table;
		return true;
	}

	template<class T>
	bool writeFile(std::string_view path, std::string_view f_name, tdb::DataTable<T>& table) {
		std::string filename = std::string{ path } + std::string{ f_name };
		LogInfo("Writing: " << filename);
		std::ofstream ofs(filename);
		if (!ofs.is_open()) {
			LogWarning("failed to open " << filename);
			return false;
		}
		boost::archive::csv_oarchive oa(ofs);
		std::string header = oa.generateHeader(T());
		oa& BOOST_SERIALIZATION_NVP(header);
		oa << BOOST_SERIALIZATION_NVP(table);
		return true;
	}
};

} // namespace tdb
