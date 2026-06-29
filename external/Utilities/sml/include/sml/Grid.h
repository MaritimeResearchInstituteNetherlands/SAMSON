// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file Grid.h
 * 
 * @brief This file contains the declaration of the Grid class.
 */
#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include <stdexcept>

#include <sml/Dimension.h>

namespace sml {

/**
	* @class Grid
	* 
	* @brief This class represents a 2D grid of cells.
	* 
	* The grid is represented as a 1D vector of cells, where each cell is a template type T. 
	* Cells can be accessed using the () operator, with the x and y coordinates of the cell.
	* 
	*/
template <typename T>
class Grid {
public:
	/**
		* @brief Constructs a grid with the specified number of columns and rows.
		* 
		* @param[in] cols The number of columns in the grid.
		* @param[in] rows The number of rows in the grid.
		* @param initial The initial value of the cells in the grid.
		*/
	Grid(size_t cols, size_t rows, const T& initial = T()) 
		: m_rows{rows}
		, m_cols{cols}
		, m_cells(rows* cols, initial)
	{
	}

	/**
		* @brief Constructs a grid with the specified number of columns and rows, and initializes the cells with the given values.
		* 
		* @param cols The number of columns in the grid.
		* @param rows The number of rows in the grid.
		* @param initials The initial values of the cells in the grid.
		*/
	Grid(size_t cols, size_t rows, std::initializer_list<T> initials) 
		: m_rows{rows}
		, m_cols{cols}
		, m_cells{initials}
	{
		//TBD: whatif rows*columns != initials.size
	}

	/**
		* @brief Sets the size of the grid and initializes the cells with the given value.
		* 
		* @param cols The number of columns in the grid.
		* @param rows The number of rows in the grid.
		* @param newT The value to initialize the cells with.
		*/
	// TODO this is not the fasted way of creating an efficient 2D grid in memory (use pointers, not index based accessor)
	void setGridSize(size_t cols, size_t rows, const T& newT = T()) {
		m_cols = cols;
		m_rows = rows;
		m_cells.assign(rows * cols, newT);
	}

	/**
		* @brief Returns the number of rows in the grid.
		* 
		* @return The number of rows in the grid.
		*/
	inline const size_t& getRows() const { return m_rows; }

	/**
		* @brief Returns the number of columns in the grid.
		* 
		* @return The number of columns in the grid.
		*/
	inline const size_t& getCols() const { return m_cols; }

	/**
		* @brief Returns the cell at the specified x and y coordinates.
		* 
		* @param i[in] The i index of the grid.
		* @param j[in] The j index of the grid.
		* @return A reference to the cell at the specified index.
		*/
	// TODO this indexed 2d structure can be improved by using an array of pointers and dereferencing them properly [x][y]
	// TODO use i and j instead of x and y
	T& operator() (size_t i, size_t j) {
		if (j >= m_rows || i >= m_cols) {
			throw std::out_of_range("Grid::operator() index out of range");
		}
		return m_cells[j * m_cols + i];
	}

	const T& operator() (size_t i, size_t j) const {
		if (j >= m_rows || i >= m_cols) {
			throw std::out_of_range("Grid::operator() index out of range");
		}
		return m_cells[j * m_cols + i];
	}


private:
	std::vector<T> m_cells;

	size_t m_cols; //->x->lon
	size_t m_rows; //->y->lat	
}; //class Grid


} // namespace sml

