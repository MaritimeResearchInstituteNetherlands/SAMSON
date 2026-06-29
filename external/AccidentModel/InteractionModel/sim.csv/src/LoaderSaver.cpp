// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/csv/LoaderSaver.h>
#include <sim/csv/OutputSchemes.h>

namespace sim
{
namespace csv
{

LoaderSaver::LoaderSaver(const InteractionModel& interactionModel, std::string directory)
	: m_interactionModel(interactionModel)
	, m_directory(directory)
{
}



void LoaderSaver::save()
{
	if (!m_interactionModel.getExposuresDone()) {
		LogInfo("Exposures have not been computed.");
	}
	if (!m_interactionModel.getEventsDone()) {
		LogInfo("Events have not been computed.");
	}
	if (!m_interactionModel.getCollisionsDone()) {
		LogInfo("Collisions have not been computed.");
	}

	saveLink();
	saveCell();
	saveLinkLink();
	saveLinkCell();
	saveLinkObject();
	saveCellObject();
}



void LoaderSaver::load()
{
	loadLink();
	loadCell();
	loadLinkLink();
	loadLinkCell();
	loadLinkObject();
	loadCellObject();
}



bool LoaderSaver::saveLink()
{
	// Set up output database
	tdb::DataTable<LinkScheme> linkResults;
	for (const auto& link : m_interactionModel.getLinks()) {
		const auto& meetings = link->getMeetings();
		for (const auto& meeting : meetings) {
			if (meeting.exposures != 0.0) {
				std::shared_ptr scheme = std::make_shared<LinkScheme>();
				scheme->m_linkUID = link->getUID();
				scheme->m_meeting = meeting;
				scheme->m_linkLength = link->getLength();
				scheme->m_trafficAUID = link->getShipLinkScheme(meeting.i).getUID();
				scheme->m_trafficBUID = link->getShipLinkScheme(meeting.j).getUID();
				linkResults.push_back(scheme);
			}
		}
	}

	return writeFile(m_directory, "link.csv", linkResults);
}



bool LoaderSaver::saveCell()
{
	// Set up output database
	tdb::DataTable<CellScheme> cellResults;
	for (const auto& cell : m_interactionModel.getCells()) {
		const auto& meetings = cell->getMeetings();
		for (const auto& meeting : meetings) {
			if (meeting.exposures != 0.0) {
				std::shared_ptr scheme = std::make_shared<CellScheme>();
				scheme->m_cellUID = cell->getUID();
				scheme->m_gridUID = cell->getGridUID();
				scheme->m_area = cell->getArea();
				scheme->m_meeting = meeting;
				scheme->m_trafficAUID = cell->getShipCellScheme(meeting.i).getUID();
				scheme->m_trafficBUID = cell->getShipCellScheme(meeting.j).getUID();
				cellResults.push_back(scheme);
			}
		}
	}

	return writeFile(m_directory, "cell.csv", cellResults);
}



bool LoaderSaver::saveLinkLink()
{
	tdb::DataTable<LinkLinkScheme> linkLinkResults;
	for (const auto& linkLink : m_interactionModel.getLinkLinks()) {
		const auto& meetings = linkLink->getMeetings();
		for (const auto& meeting : meetings) {
			if (meeting.exposures != 0.0) {
				const Link& linkA = linkLink->getLinkA();
				const Link& linkB = linkLink->getLinkB();
				std::shared_ptr scheme = std::make_shared<LinkLinkScheme>();
				scheme->m_linkAUID = linkA.getUID();
				scheme->m_linkBUID = linkB.getUID();
				scheme->m_angle = linkLink->getAngle();
				scheme->m_trafficAUID = linkA.getShipLinkScheme(meeting.i).getUID();
				scheme->m_trafficBUID = linkB.getShipLinkScheme(meeting.j).getUID();
				scheme->m_meeting = meeting;
				linkLinkResults.push_back(scheme);
			}
		}
	}
	return writeFile(m_directory, "linklink.csv", linkLinkResults);
}



bool LoaderSaver::saveLinkCell()
{
	tdb::DataTable<LinkCellScheme> linkCellResults;
	for (const auto& linkCell : m_interactionModel.getLinkCells()) {
		const auto& meetings = linkCell->getMeetings();
		for (const auto& meeting : meetings) {
			if (meeting.exposures != 0.0) {
				const Link& link = linkCell->getLink();
				const Cell& cell = linkCell->getCell();
				std::shared_ptr scheme = std::make_shared<LinkCellScheme>();
				scheme->m_linkUID = link.getUID();
				scheme->m_cellUID = cell.getUID();
				scheme->m_gridUID = cell.getGridUID();
				scheme->m_meeting = meeting;
				scheme->m_trafficAUID = link.getShipLinkScheme(meeting.i).getUID();
				scheme->m_trafficBUID = cell.getShipCellScheme(meeting.j).getUID();
				linkCellResults.push_back(scheme);
			}
		}
	}
	return writeFile(m_directory, "linkcell.csv", linkCellResults);
}



bool LoaderSaver::saveLinkObject()
{
	tdb::DataTable<LinkObjectScheme> linkObjectSchemes;
	for (const auto& linkObject : m_interactionModel.getLinkObjects()) {
		const auto& link = linkObject->getLink();
		const auto& meetings = linkObject->getMeetings();
		for (const auto& meeting : meetings) {
			std::shared_ptr scheme = std::make_shared<LinkObjectScheme>();
			scheme->m_linkUID = link.getUID();
			scheme->m_meetingIndex = meeting.index;
			scheme->m_trafficUID = link.getShipLinkScheme(meeting.i).getUID();
			scheme->m_objectUID = linkObject->getObjects()[meeting.j]->getUID();
			if (m_interactionModel.getExposuresDone()) {
				scheme->m_driftExposure = meeting.driftExposure;
				scheme->m_ramExposure = meeting.ramExposure;
			}
			if (m_interactionModel.getEventsDone()) {
				scheme->m_driftEvents = meeting.driftEvents;
				scheme->m_ramEvents = meeting.ramEvents;
			}
			if (m_interactionModel.getCollisionsDone()) {
				scheme->m_driftCollisions = meeting.driftCollisions;
				scheme->m_ramCollisions = meeting.ramCollisions;
			}
			if (meeting.ramExposure != 0.0 || meeting.driftExposure != 0.0) {
				linkObjectSchemes.push_back(scheme);
			}
		}
	}
	return writeFile(m_directory, "linkobject.csv", linkObjectSchemes);
}



bool LoaderSaver::saveCellObject()
{
	tdb::DataTable<CellObjectScheme> cellObjectSchemes;
	for (const auto& cellObject : m_interactionModel.getCellObjects()) {
		const auto& cell = cellObject->getCell();
		const auto& meetings = cellObject->getMeetings();
		for (const auto& meeting : meetings) {
			std::shared_ptr scheme = std::make_shared<CellObjectScheme>();
			scheme->m_cellUID = cell.getUID();
			scheme->m_meetingIndex = meeting.index;
			scheme->m_trafficUID = cell.getShipCellScheme(meeting.i).getUID();
			scheme->m_objectUID = cellObject->getObjects()[meeting.j]->getUID();
			if (m_interactionModel.getExposuresDone()) {
				scheme->m_driftExposure = meeting.driftExposure;
				scheme->m_ramExposure = meeting.ramExposure;
			}
			if (m_interactionModel.getEventsDone()) {
				scheme->m_driftEvents = meeting.driftEvents;
				scheme->m_ramEvents = meeting.ramEvents;
			}
			if (m_interactionModel.getCollisionsDone()) {
				scheme->m_driftCollisions = meeting.driftCollisions;
				scheme->m_ramCollisions = meeting.ramCollisions;
			}
			if (meeting.ramExposure != 0.0 || meeting.driftExposure != 0.0) {
				cellObjectSchemes.push_back(scheme);
			}
		}
	}
	return writeFile(m_directory, "cellobject.csv", cellObjectSchemes);
}



bool LoaderSaver::loadLink()
{
	tdb::DataTable<LinkScheme> linkResults;
	readFile(m_directory, "link", linkResults);
	// TODO load the data onto the meeting structure
	return true;
}



bool LoaderSaver::loadCell()
{
	// TODO
	return true;
}



bool LoaderSaver::loadLinkLink()
{
	// TODO
	return true;
}



bool LoaderSaver::loadLinkCell()
{
	// TODO
	return true;
}



bool LoaderSaver::loadLinkObject()
{
	// TODO
	return true;
}



bool LoaderSaver::loadCellObject()
{
	// TODO
	return true;
}

} // namespace csv
} // namespace sim
