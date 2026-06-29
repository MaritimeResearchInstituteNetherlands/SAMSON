// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file IModel.h
 * @brief This file contains the interface for data, data table, indexed data table and input data tables.
 * 
 */


#pragma once 

#include "tdb_export.h"
#include <sio/logger/api.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string_view>
#include <stdexcept>

namespace sml {
class Coordinate2D;
}

namespace tdb {

	/**
	 * @typedef Data 
	 * @brief Shared pointer to define a record of a table.
	 */
	template<class T>
	using Data = std::shared_ptr<T>;

	/**
	 * @typedef Data Table
	 * @brief List of data to define a table.
	 */
	template<class T>
	using DataTable = std::vector<Data<T>>;

	/**
	 * @class IndexedDataTable
	 * @brief A table with an index to access data by key from data table.
	 */
	template<class T, class UIDType>
	class IndexedDataTable {
	public:
		using DataType = Data<T>;

		/**
		 * @brief Function to add data to the indexed table with a key.
		 * 
		 * If the key already exists, throws exception.
		 * 
		 * @param[in] data The data to be added and indexed
		 * @param[in] key The key to insert the data
		 */
		void add(const DataType& data, const UIDType& key) {
			if (m_index.find(key) != m_index.end()) {								
				throw std::invalid_argument("Key already exists");
			}
			m_dataList.push_back(data);
			m_index[key] = m_dataList.size() - 1;
		}



		/**
		 * @brief Function to add full data table to the indexed table.
		 * 
		 * If any key already exists, throws exception.
		 * 
		 * @param[in] dataTable The data table to be added and indexed
		 */
		template<class DataTable>
		void add(const DataTable& dataTable) {
			for (auto& data : dataTable) {
				if (m_index.find(data->getUID()) != m_index.end()) {
					LogFatal("Unable to add dataTable. Key " << data->getUID() << " of type " << typeid(data).name() << " already exists.");
				}
				m_dataList.push_back(data);
				m_index[data->getUID()] = m_dataList.size() - 1;
			}			
		}



		/**
		 * @brief Function to remove data by key.
		 * 
		 * Tries to optimize removal by swapping the element to be removed with the last element.
		 * If the key does not exist, does nothing.
		 * 
		 * @param[in] key The key to remove the data
		 */
		void remove(const UIDType& key) {
			auto it = m_index.find(key);
			if (it == m_index.end()) {				
				return;
			}

			//Keep the index of the item to be removed
			size_t idx = it->second;
			//Remove item index
			m_index.erase(it);

			//If the item to be removed is not the last element of the vector
			if (idx != m_dataList.size() - 1) {
				//Swap the element to be removed with the last element :
				std::swap(m_dataList[idx], m_dataList.back());
				//Find the key of the last element that was swapped to the location of item to be removed
				auto swappedKey = std::find_if(m_index.begin(), m_index.end(),
					[this](const auto& pair) { return pair.second == m_dataList.size() - 1; });
				//Update the index of the swapped element
				if (swappedKey != m_index.end()) {
					swappedKey->second = idx;
				}
			}
			//Here the item to be removed is at the end of the vector, just remove it
			m_dataList.pop_back();
		}



		/**
		 * @brief Function to get data by key.
		 * 
		 * If the key does not exist, returns nullptr.
		 * 
		 * @param[in] key The key to get the data
		 */
		DataType get(const UIDType& key) const {
			auto it = m_index.find(key);
			if (it != m_index.end()){
				return m_dataList[it->second];
			}			
			LogError("Can not find data entry with key " << key);
			return nullptr;
		}



		/**
		 * @brief Function to get data by index.
		 * 
		 * If the index is out of range, returns nullptr.
		 * 
		 * @param[in] idx The index to get the data
		 */
		DataType get(const size_t& idx) const {
			if (idx < m_dataList.size())
			{
				return m_dataList[idx];
			}	
			return nullptr;
		}



		/**
		 * @brief Function to get the index of the key.
		 * 
		 * If the key does not exist, returns the size of the data list.
		 * 
		 * @param[in] key The key to get the index
		 */
		size_t getIndex(const UIDType& key) const {
			auto it = m_index.find(key);
			if (it != m_index.end()){
				return it->second;
			}		
			LogError("Can not find data index with key " << key);
			return m_dataList.size();
		}



		void clear()
		{
			m_dataList.clear();
			m_index.clear();
		}



		inline const std::vector<DataType>& getIndexedData() const { return m_dataList; }

		inline const size_t size() const { return m_dataList.size(); }
	private:
		std::vector<DataType> m_dataList;
		std::unordered_map<UIDType, size_t> m_index;	
	};


	/**
	 * @typedef MergeDataTable
	 * 
	 * @brief Used to keep a list of data tables with a key. Mostly used for merged data tables.
	 */
	//template<class K, class T>
	//using MergeDataTable = std::unordered_map<K, DataTable<T>>;

	//template<class T>
	//class WaypointOld;
	//class LinkScheme;
	//class TrafficNetwork;
	//class ShipCategory;
	//class HarmonicSpeed;
	//class ShipLink;
	//class TrafficDB;
	////class GridObject;
	//class Area;

	/**
	 * @struct DataTables
	 * @brief A struct to hold all input trafficdb data tables.
	 * 
	 * TBD: Shall be moved dto trafficdb folder
	 */
	//struct TDB_EXPORT DataTables {
	//	DataTable<WaypointOld<sml::Coordinate2D>> waypoints;
	//	DataTable<LinkScheme> links;
	//	DataTable<TrafficNetwork> trafficNetworks;
	//	DataTable<ShipCategory> shipCategories;
	//	DataTable<HarmonicSpeed> harmonicSpeeds;
	//	DataTable<ShipLink> shipLinks;
	//	DataTable<TrafficDB> trafficDBs;
	//	DataTable<Area> areas;
	//
	//	//TBD: Get tables from db
	//	bool readAllTables(std::string_view path);		
	//	bool readAllTables(std::string_view path, std::unordered_map<std::string, std::string>&& tables);
	//};
} // namespace tdb

