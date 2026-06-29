// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <filesystem>

#include <sio.shapelib_export.h>
#include <shapelib/shapefil.h>


namespace sio {
namespace shapelib {

enum class shapeType {
	point = 1,
	arc = 3,
	polygon = 5
};



template <class T>
class Attribute
{
public:
	Attribute(std::string name, std::function<const T(size_t)> getValue)
		: m_name(name)
		, m_getValue(getValue)
	{}

	inline std::string getName() { return m_name; }

	inline const T getValue(const size_t index) { return m_getValue(index); }
private:
	std::string m_name;
	std::function<const T(const size_t)> m_getValue;
};

using StringAttribute = Attribute<std::string>;
using DoubleAttribute = Attribute<double>;
using IntegerAttribute = Attribute<int>;



class SIO_SHAPELIB_EXPORT ObjectWriter
{
public:
	ObjectWriter() {}

	virtual const std::vector<std::pair<double, double>>& getCoordinates(size_t index) = 0;

	inline void addStringAttribute(std::string name, std::function<const std::string(size_t)> getValue) { m_stringAttributes.push_back(Attribute<std::string>(name, getValue)); }

	inline void addDoubleAttribute(std::string name, std::function<const double(size_t)> getValue) { m_doubleAttributes.push_back(Attribute<double>(name, getValue)); }

	inline void addIntegerAttribute(std::string name, std::function<const int(size_t)> getValue) { m_integerAttributes.push_back(Attribute<int>(name, getValue)); }

	inline const std::vector<StringAttribute>& getStringAttributes() { return m_stringAttributes; }

	inline const std::vector<DoubleAttribute>& getDoubleAttributes() { return m_doubleAttributes; }

	inline const std::vector<IntegerAttribute>& getIntegerAttributes() { return m_integerAttributes; }

	virtual sio::shapelib::shapeType getType() const = 0;

	virtual size_t size() const = 0;
private:
	std::vector<StringAttribute> m_stringAttributes;
	std::vector<DoubleAttribute> m_doubleAttributes;
	std::vector<IntegerAttribute> m_integerAttributes;
};



class SIO_SHAPELIB_EXPORT ShapelibWriter
{
public:
	ShapelibWriter(std::filesystem::path path);

	void write(const std::string& name, ObjectWriter& objectWriter);

private:
	void writeSHP(const std::string& name, ObjectWriter& objectWriter);

	void writeDBF(const std::string& name, ObjectWriter& objectWriter);

	void writePRJ(const std::string& name);

	template <class T>
	void writeAttribute(DBFHandle dbfhandle, const std::string& name, size_t& attributeIndex, const size_t size, const std::vector<Attribute<T>>& attributeList)
	{
		//std::cout << "Processing attributes of " << name << std::endl;
		for (auto attribute : attributeList) {
			// Add attributes
			if (attribute.getName().size() > 10) {
				std::cout << "Warning: attribute " << attribute.getName() << " is too long, can only have 10 characters." << std::endl;
			}
			addField<const T>(dbfhandle, attribute.getName());

			// Add values
			//std::cout << "Attribute values: " << std::endl;
			for (size_t index = 0; index < size; index++) {
				//std::cout << attribute.getValue(index) << std::endl;
				writeField<const T>(dbfhandle, index, attributeIndex, attribute.getValue(index));
			}
			attributeIndex++;
		}
	}

	template <class T>
	void addField(DBFHandle dbfhandle, const std::string& attributeName) {}

	template <class T>
	void writeField(DBFHandle dbfhandle, const size_t index, const size_t attributeIndex, T value) {}

	std::filesystem::path m_path;
};



template <>
inline void ShapelibWriter::addField<const std::string>(DBFHandle dbfhandle, const std::string& attributeName)
{
	size_t maxStringSize = 100;
	DBFAddField(dbfhandle, attributeName.c_str(), FTString, maxStringSize, 0);
}



template <>
inline void ShapelibWriter::addField<const double>(DBFHandle dbfhandle, const std::string& attributeName)
{
	// TODO set something sensible here
	DBFAddField(dbfhandle, attributeName.c_str(), FTDouble, 16, 16);
}



template <>
inline void ShapelibWriter::addField<const int>(DBFHandle dbfhandle, const std::string& attributeName)
{
	// TODO set something sensible here
	DBFAddField(dbfhandle, attributeName.c_str(), FTInteger, 16, 0);
}



template <>
inline void ShapelibWriter::writeField<const std::string>(DBFHandle dbfhandle, const size_t index, const size_t attributeIndex, const std::string value)
{
	DBFWriteStringAttribute(dbfhandle, index, attributeIndex, value.c_str());
}



template <>
inline void ShapelibWriter::writeField<const double>(DBFHandle dbfhandle, const size_t index, const size_t attributeIndex, const double value)
{
	DBFWriteDoubleAttribute(dbfhandle, index, attributeIndex, value);
}




template <>
inline void ShapelibWriter::writeField<const int>(DBFHandle dbfhandle, const size_t index, const size_t attributeIndex, const int value)
{
	DBFWriteIntegerAttribute(dbfhandle, index, attributeIndex, value);
}

} // namespace shapelib
} // namespace sio
