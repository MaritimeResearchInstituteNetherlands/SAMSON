// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/Area.h>

using namespace tdb;


//Area::Area( )
//	: m_uid{ "" }
//	, m_start{0,0}
//	, m_end{0,0}
//	, m_grid{1,1}
//{
//	m_extent = sml::GeoUtils::toPoint(m_start, m_end);
//	m_cellSizeMeter = { m_extent.x, m_extent.y };
//	//There is only one cell, whole area	
//}
//
//
//Area::Area(std::string_view uid, const sml::Coordinate2D& start, const sml::Coordinate2D& end, const sml::OriginDirFrom& originDirFrom)
//	: m_uid{uid}
//	, m_start{start}
//	, m_end{end}
//	, m_originDirFrom{originDirFrom}
//	, m_grid{1,1}
//{	
//	sml::GeoUtils::updateOrientation(m_start, m_end, m_originDirFrom);
//	m_extent = sml::GeoUtils::toPoint(m_start, m_end);
//	m_cellSizeMeter = { m_extent.x, m_extent.y };
//	//There is only one cell, whole area	
//}
//
////Note that, grid cells of area simply holding origin of each cell.
//// The default origin of an rea is the most South-West point. Cell origin and area origin position are same.
//sml::Dimension2D Area::updateCells(unsigned short gridNumX, unsigned short gridNumY) {	
//	m_grid.setGridSize(gridNumX, gridNumY);
//	m_cellSizeMeter = sml::Dimension2D(m_extent.x / gridNumX, m_extent.y / gridNumY);
//	return refreshGridCells();
//}
//
//sml::Dimension2D Area::setCellSize(float xLength, float yLength) {
//	sml::GeoUtils::updateOrientation(m_start, m_end, m_originDirFrom);
//	m_extent = sml::GeoUtils::toPoint(m_start, m_end);
//	m_cellSizeMeter = sml::Dimension2D{ xLength, yLength };
//	size_t x_size = std::max(1, static_cast<int>(round(m_extent.x / xLength)));	
//	size_t y_size = std::max(1, static_cast<int>(round(m_extent.y / yLength)));
//	m_grid.setGridSize(x_size, y_size);
//	return refreshGridCells();
//}
//
//sml::Dimension2D Area::refreshGridCells() {
//	double meterX = 0.0;
//	double meterY = 0.0;
//	for (auto x = 0; x < m_grid.getCols(); ++x) {
//		for (auto y = 0; y < m_grid.getRows(); ++y) {
//			m_grid(x, y) = sml::Cell2D(sml::Point2D(meterX, meterY));
//			meterY += m_cellSizeMeter.y;
//		}
//		meterX += m_cellSizeMeter.x;
//		meterY = 0.0;
//	}
//	return m_cellSizeMeter;
//}

