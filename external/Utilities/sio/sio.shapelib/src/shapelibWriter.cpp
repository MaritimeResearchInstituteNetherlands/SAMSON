// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sio/shapelib/shapelibWriter.h>

#include "sio.shapelib_export.h"
#include <fstream>

namespace sio {
namespace shapelib {


ShapelibWriter::ShapelibWriter(std::filesystem::path path)
	: m_path(path)
{
}




void ShapelibWriter::write(const std::string& name, ObjectWriter& objectWriter)
{
	// Write shape file
	writeSHP(name, objectWriter);

	// Database attributes
	writeDBF(name, objectWriter);

	// Projection file
	writePRJ(name);
}



void ShapelibWriter::writeSHP(const std::string& name, ObjectWriter& objectWriter)
{
	// Obtain general shapefile information
	size_t numObjects = objectWriter.size();
	size_t type = static_cast<size_t>(objectWriter.getType());

	// Create data vectors
	std::vector<double> xval;
	std::vector<double> yval;
	std::vector<double> zval;
	std::vector<double> mval;

	// Create shape file
	std::string filename = m_path.string() + "\\" + name;
	SHPHandle handle = SHPCreate(filename.c_str(), type);
	size_t index = 0;
	size_t pos = 0;
	for (size_t index = 0; index < objectWriter.size(); index++) {
		//std::cout << "writing " << name << ", " << index << std::endl;
		const std::vector<std::pair<double, double>>& coordinates = objectWriter.getCoordinates(index);

		// Fill in the data
		for (size_t i = 0; i < coordinates.size(); i++) {
			//std::cout << "Writing coordinates " << coordinates[i].first << ", " << coordinates[i].second << std::endl;
			xval.push_back(coordinates[i].second);
			yval.push_back(coordinates[i].first);
			zval.push_back(0.0);
			mval.push_back(0.0);
		}
		//std::cout << "pos: " << pos << std::endl;

		// Add shape object
		SHPObject* obj = SHPCreateObject(type, -1, 0, nullptr, nullptr, coordinates.size(), xval.data() + pos, yval.data() + pos, zval.data() + pos, mval.data() + pos);
		SHPWriteObject(handle, -1, obj);
		SHPDestroyObject(obj);

		// Update counters
		pos += coordinates.size();
	}
	SHPClose(handle);
}



void ShapelibWriter::writeDBF(const std::string& name, ObjectWriter& objectWriter)
{
	// Initialise
	std::string filename = m_path.string() + "\\" + name;
	DBFHandle dbfhandle = DBFCreate(filename.c_str());
	size_t attributeIndex = 0;
	
	// Write attributes, each attribute increases attributeIndex
	writeAttribute<std::string>(dbfhandle, name, attributeIndex, objectWriter.size(), objectWriter.getStringAttributes());
	writeAttribute<double>(dbfhandle, name, attributeIndex, objectWriter.size(), objectWriter.getDoubleAttributes());
	writeAttribute<int>(dbfhandle, name, attributeIndex, objectWriter.size(), objectWriter.getIntegerAttributes());

	// Finalise
	DBFClose(dbfhandle);
}



void ShapelibWriter::writePRJ(const std::string& name)
{
	std::string filename = m_path.string() + "\\" + name + ".prj";
	std::ofstream file(filename);

	file << "GEOGCS[\"GCS_WGS_1984\", DATUM[\"D_WGS_1984\", SPHEROID[\"WGS_1984\", 6378137.0, 298.257223563]], PRIMEM[\"Greenwich\", 0.0], UNIT[\"Degree\", 0.0174532925199433]]";
}

} // namespace sio
} // namespace shapelib
