// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <tdb/shapelib/writer.h>
#include <tdb/shapelib/objectWriters.h>

namespace tdb {
namespace shapelib {

Writer::Writer(std::filesystem::path path, const AreaModel& areaModel)
{
	sio::shapelib::ShapelibWriter writer(path);

	// Construct object writers
	WaypointObjectWriter waypointObjectWriter(areaModel.getWayPoints(), areaModel.getVertices());
	LinkObjectWriter linkObjectWriter(areaModel.getLinks(), areaModel.getWayPoints(), areaModel.getVertices());
	CellObjectWriter cellObjectWriter(areaModel.getCells(), areaModel.getVertices());
	ObjectObjectWriter objectObjectWriter(areaModel.getObjects(), areaModel.getVertices());
	ZoneObjectWriter zoneObjectWriter(areaModel.getZones(), areaModel.getVertices());
	WeatherstationObjectWriter weatherstationObjectWriter(areaModel.getWeatherstations(), areaModel.getVertices());
	ErtvObjectWriter ertvObjectWriter(areaModel.getErtvs(), areaModel.getVertices());

	// Write all objects
	writer.write("waypoints", waypointObjectWriter);
	writer.write("links", linkObjectWriter);
	writer.write("cells", cellObjectWriter);
	writer.write("objects", objectObjectWriter);
	writer.write("zones", zoneObjectWriter);
	writer.write("weatherstations", weatherstationObjectWriter);
	writer.write("ertvs", ertvObjectWriter);

}

} // namespace shapelib
} // namespace tdb
