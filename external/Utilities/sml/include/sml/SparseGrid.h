// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file SparseGrid.h
 * @brief This file contains the declaration of the SparseGrid class.
 */
#pragma once

#include <sml_export.h>

#include <vector>
#include <unordered_map>
#include <utility>

#include <sml/Dimension.h>

namespace sml {

/**
	* @struct SizePairHash
	* @brief Hash function for a pair of size_t values used as a key in the SparseGrid.
	* 
	* This struct overrides operator() to calculate the hash value of a pair of size_t values.
	* So that the pair can be used as a key in the SparseGrid unordered_map.
	*/
struct SizePairHash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2>& p) const {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ h2;
	}
}; // struct SizePairHash 

/**
	* @struct SizePairEqual
	* @brief Equality function for a pair of size_t values used as a key in the SparseGrid.
	* 
	* This struct overrides operator() to compare two pairs of size_t values for equality.
	* So that the pair can be used as a key in the SparseGrid unordered_map.
	*/
struct SizePairEqual {
	template <class T1, class T2>
	bool operator () (const std::pair<T1, T2>& p1, const std::pair<T1, T2>& p2) const {
		return (p1.first == p2.first && p1.second == p2.second);
	}
}; // struct SizePairEqual

/**
	* @class SparseGrid
	* @brief This class represents a 2D grid of cells with sparse data.
	* 
	* The grid is represented as a 2D map of cells, where each cell is a template type T.
	* The grid is sparse, meaning that only cells with non-default values are stored in the map.
	* Cells can be accessed using the () operator, with the x and y coordinates of the cell. 
	* There is also an iterator to iterate over the cells in the grid with begin and end functions.
	*/
template <typename T>
class SparseGrid {
public:
	/**
		* @brief Iterator type for the SparseGrid.
		*/
	using SparseGridIterator = std::unordered_map<std::pair<size_t, size_t>, T, SizePairHash, SizePairEqual>::iterator;

	/**
		* @brief Function to get the value of a cell at the specified i and j grid indices.
		* 
		* @param i The x coordinate of the cell.
		* @param j The y coordinate of the cell.
		* @return The value of the cell at the specified i and j index.
		*/
	T& x_y_access(size_t i, size_t j) {
		auto key = std::make_pair(i, j);
		return m_sparseCells[key];
	}

	/**
		* @brief Function to get the value of a cell at the specified i and j grid indices.
		* 
		* @param i The i index of the cell.
		* @param j The j index of the cell.
		* @return The value of the cell at the specified i and j grid index.
		*/
	T& operator()(size_t x, size_t y) {
		auto key = std::make_pair(x, y);
		return m_sparseCells[key];
	}
		
	/**
		* @brief Function to get the value of a cell at the specified key.
		* 
		* @param key The key of the cell which is a pair x and y coordinates.
		* @return The value of the cell at the specified key.
		*/
	T& operator()(const std::pair<size_t, size_t>& key) {			
		return m_sparseCells[key];
	}

	/**
		* @brief Function to get the start iterator of the SparseGrid.
		* 
		* @return The start iterator of the SparseGrid.
		*/
	SparseGridIterator begin() {
		return m_sparseCells.begin();
	}

	/**
		* @brief Function to get the end iterator of the SparseGrid.
		* 
		* @return The end iterator of the SparseGrid.
		*/
	SparseGridIterator end() {
		return m_sparseCells.end();
	}

	/**
		* @brief Function to serialize the SparseGrid object.
		* 
		* @param[in] Archive The archive to serialize the SparseGrid object.
		* @param[in] version The version of the serialization (not used).
		*/
	//template<class Archive>
	//void save(Archive& ar, const unsigned int version) const
	//{
	//	for (auto c = m_sparseCells.begin(); c != m_sparseCells.end(); ++c) {
	//		ar& boost::serialization::make_nvp("x", c->first.first);
	//		ar& boost::serialization::make_nvp("y", c->first.second);
	//		ar& boost::serialization::make_nvp("values", c->second);
	//		//TBD: should not be dependendt to csv!
	//		if (std::distance(c, m_sparseCells.end()) > 1) {
	//			ar& CSV_OARCHIVE_NL();
	//		}
	//	}
	//}

	/**
		* @brief Function to deserialize the SparseGrid object.
		* 
		* @param[in] Archive The archive to deserialize the SparseGrid object.
		* @param[in] version The version of the serialization (not used).
		*/
	//template<class Archive>
	//void load(Archive& ar, const unsigned int version)
	//{
	//		
	//	size_t i, j;
	//	boost::archive::csv_i_flags nlFlag;			
	//	do {
	//		T values;
	//		ar& BOOST_SERIALIZATION_NVP(i);
	//		ar& BOOST_SERIALIZATION_NVP(j);
	//		ar& BOOST_SERIALIZATION_NVP(values);
	//		(*this)(i, j) = values;
	//		//Read new at next loop
	//		ar& BOOST_SERIALIZATION_NVP(nlFlag);								
	//	} while (nlFlag != boost::archive::csv_i_flags::endOfFile);
	//
	//
	//}

	//BOOST_SERIALIZATION_SPLIT_MEMBER()
private:
	std::unordered_map<std::pair<size_t, size_t>, T, SizePairHash, SizePairEqual> m_sparseCells;

};

} // namespace sml

