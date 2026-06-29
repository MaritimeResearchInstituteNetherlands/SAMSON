// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/shapelib/writer.h>
#include <sim/shapelib/objectWriters.h>

namespace sim {
namespace shapelib {


// Area writer. TODO is this really output data or is it considered to be still sort-of input? it does not need anything from interactionModel..
AreaWriter::AreaWriter(std::filesystem::path path, const std::vector<std::unique_ptr<Link>>& links)
{
	sio::shapelib::ShapelibWriter writer(path);

	LinkPolygonWriter linkPolygonWriter(links);
	LinkRectangleWriter linkRectagnleWriter(links);

	writer.write("linkPolygon", linkPolygonWriter);
	writer.write("linkRectangle", linkRectagnleWriter);
}



ResultWriter::ResultWriter(std::filesystem::path path, const tdb::AreaModel& areaModel, const sim::InteractionModel& model)
{
	sio::shapelib::ShapelibWriter writer(path);

	OvertakingResultsWriter overtakingResultsWriter(model, areaModel.getLinks(), areaModel.getWayPoints(), areaModel.getVertices());
	CrossingResultsWriter linkLinkCrossingResultsWriter(model, areaModel.getWayPoints(), areaModel.getVertices());
	HeadonResultsWriter headonResultWriter(model, areaModel.getLinks(), areaModel.getWayPoints(), areaModel.getVertices());
	DriftResultWriter driftResultsWriter(model, areaModel.getObjects(), areaModel.getVertices());
	RamResultWriter ramResultsWriter(model, areaModel.getObjects(), areaModel.getVertices());

	writer.write("overtakingResults", overtakingResultsWriter);
	writer.write("crossingResults", linkLinkCrossingResultsWriter);
	writer.write("headonResults", headonResultWriter);
	writer.write("driftResults", driftResultsWriter);
	writer.write("ramResults", ramResultsWriter);
}

} // namespace shapelib
} // namespace sim
