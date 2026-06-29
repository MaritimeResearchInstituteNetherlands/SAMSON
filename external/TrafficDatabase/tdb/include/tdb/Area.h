// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/**
 * @file Area.h
 * @brief This file contains the declaration of the Area class.
 */

#pragma once

#include "tdb_export.h"

#include "util/IIdentifiable.h"
#include "util/Types.h"
#include <sml/GeoUtils.h>
#include <sml/Dimension.h>
#include <sml/Cell.h>
#include <sml/Grid.h>

#include <string_view>
#include <vector>
#include <memory>


namespace tdb {

/**
	* @class Area
	* @brief Defines an area with a grid of cells.
	* 
	*/
//class TDB_EXPORT Area : public IIdentifiable {
//public:
//	/**
//		* @brief Default constructor
//		*/
//	Area();
//		
//	/**
//		* @brief Constructor to create an area with a unique identifier, start and end coordinates, 
//		* and the direction from which the origin is defined.
//		* 
//		* @param[in] uid The unique identifier of the area
//		* @param[in] start The start coordinate of the area
//		* @param[in] end The end coordinate of the area
//		* @param[in] originDirFrom The direction from which the origin is defined, default is { LonDir::West, LatDir::South }
//		*/
//	Area(std::string_view uid, const sml::Coordinate2D& start, const sml::Coordinate2D& end,
//		const sml::OriginDirFrom& originDirFrom = { sml::LonDir::West, sml::LatDir::South });
//
//	/**
//		* @brief Function to update the grid cells of the area.
//		* 
//		* Sets the number of grid cells in the x and y directions and updates the grid cell size. 
//		* It also refreshes the grid cell positions regarding the new cell size and origin by calling refreshGridCells().
//		* 
//		*/
//	sml::Dimension2D updateCells(unsigned short gridNumX, unsigned short gridNumY);
//
//	/**
//		* @brief Function to set the cell size of the area.
//		*/
//	sml::Dimension2D setCellSize(float xLength, float yLength);
//
//	/**
//		* @brief Function updates the cell positions based on the cell size and the origin of the area.
//		* 		 
//		*/
//	sml::Dimension2D refreshGridCells();
//
//	/**
//		* @brief Function to get the origin coordinates of the area.
//		*/
//	inline const sml::Coordinate2D& getOriginCoord() const { return m_start; }
//
//	/**
//		* @brief Function to get the end coordinates of the area.
//		*/
//	inline const sml::Coordinate2D& getEndCoord() const { return m_end; }
//
//	/**
//		* @brief Function to get the size of the area in meter.
//		*/
//	inline const sml::Point2D& getExtent() const { return m_extent; }
//
//	/**
//		* @brief Function to get the cell size in meter.
//		*/
//	inline const sml::Dimension2D& getCellSizeMeter() const { return m_cellSizeMeter; }
//
//	/**
//		* @brief Function to get the cell size in meter.
//		*/
//	inline const sml::Grid<sml::Cell2D>& getGrid() const { return m_grid; }
//
//	/**
//		* @brief Function to get unique identifier of the area.
//		*/
//	inline const std::string getUID() const override { return m_uid; }
//
//	/**
//		* @brief Function to serialize and deserialize the area object.
//		* 
//		* It is difficult to calculate extent(size) of the area for user. 
//		* Therefore, extent is calculated from provided cell size, start and end points of the area
//		* while loading (deserializing) the area object.
//		* 
//		* @param[in] ar The archive to serialize and deserialize the area object
//		* @param[in] version The version of the serialization (not used)
//		*/
//	template<class Archive>
//	void serialize(Archive& ar, const unsigned int version)
//	{
//		//ar& BOOST_SERIALIZATION_NVP(m_uid);
//		//ar& BOOST_SERIALIZATION_NVP(m_start.lat);
//		//ar& BOOST_SERIALIZATION_NVP(m_start.lon);
//		//ar& BOOST_SERIALIZATION_NVP(m_end.lat);
//		//ar& BOOST_SERIALIZATION_NVP(m_end.lon);
//		//ar& BOOST_SERIALIZATION_NVP(m_originDirFrom.latDir);
//		//ar& BOOST_SERIALIZATION_NVP(m_originDirFrom.lonDir);
//		//ar& BOOST_SERIALIZATION_NVP(m_extent.x);
//		//ar& BOOST_SERIALIZATION_NVP(m_extent.y);
//		//ar& BOOST_SERIALIZATION_NVP(m_cellSizeMeter.x);
//		//ar& BOOST_SERIALIZATION_NVP(m_cellSizeMeter.y);
//		//TBD: refreshing grid is unnecessary while saving
//		this->setCellSize(m_cellSizeMeter.x, m_cellSizeMeter.y);
//	}
//
//private:		
//	Area_UidType m_uid;
//	sml::Coordinate2D m_start;
//	sml::Coordinate2D m_end;		
//	sml::OriginDirFrom m_originDirFrom;
//	sml::Point2D m_extent;
//		
//	sml::Dimension2D m_cellSizeMeter;
//	sml::Grid<sml::Cell2D> m_grid;
//}; //class Area
	
} //namespace tdb
