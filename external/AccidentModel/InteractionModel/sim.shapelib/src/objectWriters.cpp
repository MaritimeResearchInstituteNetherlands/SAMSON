// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sim/shapelib/objectWriters.h>
#include <sml/GeoUtils.h>

namespace sim {
namespace shapelib {

LinkPolygonWriter::LinkPolygonWriter(const std::vector<std::unique_ptr<Link>>& links)
	: m_links(links)
{
	addStringAttribute("link_uid", [&](const size_t index) -> const std::string { return m_links[index]->getUID(); });
}



const std::vector<std::pair<double, double>>& LinkPolygonWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();

	const auto& polygon = m_links[index]->getPolygon();
	for (const auto& coord : polygon) {
		m_coordinates.push_back({ coord.lat, coord.lon });
	}
	return m_coordinates;
}



LinkRectangleWriter::LinkRectangleWriter(const std::vector<std::unique_ptr<Link>>& links)
	: m_links(links)
{
	addStringAttribute("link_uid", [&](const size_t index) -> const std::string { return m_links[index]->getUID(); });
}



const std::vector<std::pair<double, double>>& LinkRectangleWriter::getCoordinates(size_t index)
{
	m_coordinates.clear();
	const auto& rectangle = m_links[index]->getRectangle();
	for (const auto& coord : rectangle) {
		m_coordinates.push_back({ coord.lat, coord.lon });
	}

	return m_coordinates;
}



OvertakingResultsWriter::OvertakingResultsWriter(const InteractionModel& model, const tdb::IndexedLinks& links, const tdb::IndexedWaypoints& waypoints, const tdb::IndexedVertices& vertices)
	: Base(links, waypoints, vertices)
{
	addStringAttribute("uid", [&](const size_t index) -> const std::string { return m_links.get(index)->getUID(); });
	addDoubleAttribute("exposures", [&] (const size_t index) -> const double 
		{ 
			double exposure = 0.0;
			for (const auto& link : model.getLinks()) {
				if (link->getUID() == m_links.get(index)->getUID()) {
					for (const auto& meeting : link->getMeetings()) {
						exposure += meeting.exposures;
					}
				}
			}
			return exposure;
		});
	addDoubleAttribute("events", [&](const size_t index) -> const double
		{
			double events = 0.0;
			for (const auto& link : model.getLinks()) {
				if (link->getUID() == m_links.get(index)->getUID()) {
					for (const auto& meeting : link->getMeetings()) {
						events += meeting.events_i + meeting.events_j;
					}
				}
			}
			return events;
		});
	addDoubleAttribute("collisions", [&](const size_t index) -> const double
		{
			double collisions = 0.0;
			for (const auto& link : model.getLinks()) {
				if (link->getUID() == m_links.get(index)->getUID()) {
					for (const auto& meeting : link->getMeetings()) {
						collisions += meeting.collisions_i + meeting.collisions_j;
					}
				}
			}
			return collisions;
		});
}



CrossingResultsWriter::CrossingResultsWriter(const sim::InteractionModel& model, const tdb::IndexedWaypoints& waypoints, const tdb::IndexedVertices& vertices)
	: Base(waypoints, vertices)
{
	addDoubleAttribute("exposures", [&](const size_t index) -> const double { 
			double exposure = 0.0;
			for (const auto& linkLink : model.getLinkLinks()) {
				const auto& linkA = linkLink->getLinkA();
				const auto& linkB = linkLink->getLinkB();
				if (linkA.getWaypointEnd() == m_waypoints.get(index)) { // Filter crossings at this waypoint
					if (linkA.getWaypointEnd() == linkB.getWaypointEnd()) { // Crossing criteria
						for (const auto& meeting : linkLink->getMeetings()) {
							exposure += meeting.exposures;
						}
					}
				}
			}
			return exposure;
		}
	);
	addDoubleAttribute("events", [&](const size_t index) -> const double {
		double events = 0.0;
		for (const auto& linkLink : model.getLinkLinks()) {
			const auto& linkA = linkLink->getLinkA();
			const auto& linkB = linkLink->getLinkB();
			if (linkA.getWaypointEnd() == m_waypoints.get(index)) { // Filter crossings at this waypoint
				if (linkA.getWaypointEnd() == linkB.getWaypointEnd()) { // Crossing criteria
					for (const auto& meeting : linkLink->getMeetings()) {
						events += meeting.events_i + meeting.events_j;
					}
				}
			}
		}
		return events;
		}
	);
	addDoubleAttribute("collisions", [&](const size_t index) -> const double {
		double collisions = 0.0;
		for (const auto& linkLink : model.getLinkLinks()) {
			const auto& linkA = linkLink->getLinkA();
			const auto& linkB = linkLink->getLinkB();
			if (linkA.getWaypointEnd() == m_waypoints.get(index)) { // Filter crossings at this waypoint
				if (linkA.getWaypointEnd() == linkB.getWaypointEnd()) { // Crossing criteria
					for (const auto& meeting : linkLink->getMeetings()) {
						collisions += meeting.collisions_i + meeting.collisions_j;
					}
				}
			}
		}
		return collisions;
		}
	);
}



HeadonResultsWriter::HeadonResultsWriter(const InteractionModel& model, const tdb::IndexedLinks& links, const tdb::IndexedWaypoints& waypoints, const tdb::IndexedVertices& vertices)
	: Base(links, waypoints, vertices)
{
	addDoubleAttribute("exposures", [&](const size_t index) -> const double {
		double exposure = 0.0;
		for (const auto& linkLink : model.getLinkLinks()) {
			if (linkLink->getLinkA().getWaypointStart() == linkLink->getLinkB().getWaypointEnd()) { // Test for head-on
				if (m_links.get(index)->getUID() == linkLink->getLinkB().getUID()) { // Test for this link
					for (const auto& meeting : linkLink->getMeetings()) {
						exposure += meeting.exposures;
					}
				}
			}
		}
		return exposure;
		}
	);
	addDoubleAttribute("events", [&](const size_t index) -> const double {
		double events = 0.0;
		for (const auto& linkLink : model.getLinkLinks()) {
			if (linkLink->getLinkA().getWaypointStart() == linkLink->getLinkB().getWaypointEnd()) { // Test for head-on
				if (m_links.get(index)->getUID() == linkLink->getLinkB().getUID()) {
					for (const auto& meeting : linkLink->getMeetings()) {
						events += meeting.events_i + meeting.events_j;
					}
				}
			}
		}
		return events;
		}
	);
	addDoubleAttribute("collisions", [&](const size_t index) -> const double {
		double collisions = 0.0;
		for (const auto& linkLink : model.getLinkLinks()) {
			if (linkLink->getLinkA().getWaypointStart() == linkLink->getLinkB().getWaypointEnd()) { // Test for head-on
				if (m_links.get(index)->getUID() == linkLink->getLinkB().getUID()) {
					for (const auto& meeting : linkLink->getMeetings()) {
						collisions += meeting.collisions_i + meeting.collisions_j;
					}
				}
			}
		}
		return collisions;
		}
	);
}



DriftResultWriter::DriftResultWriter(const sim::InteractionModel& model, const tdb::IndexedObjects& objects, const tdb::IndexedVertices& vertices)
	: Base(objects, vertices)
{
	addStringAttribute("type", [&](const size_t index) -> const std::string { return m_objects.get(index)->getType();});
	addStringAttribute("descript", [&](const size_t index) -> const std::string { return m_objects.get(index)->getDescription(); });
	addDoubleAttribute("exposures", [&](const size_t index) -> const double {
			double exposure = 0.0;
			for (const auto& linkObject : model.getLinkObjects()) {
				for (const auto& meeting : linkObject->getMeetings()) {
					if (m_objects.get(index)->getUID() == linkObject->getObject(meeting.j)->getUID()) {
						exposure += meeting.driftExposure;
					}
				}
			}
			return exposure;
		}
	);
	addDoubleAttribute("events", [&](const size_t index) -> const double {
		double events = 0.0;
		for (const auto& linkObject : model.getLinkObjects()) {
			for (const auto& meeting : linkObject->getMeetings()) {
				if (m_objects.get(index)->getUID() == linkObject->getObject(meeting.j)->getUID()) {
					events += meeting.driftEvents;
				}
			}
		}
		return events;
		}
	);
	addDoubleAttribute("collisions", [&](const size_t index) -> const double {
		double collisions = 0.0;
		for (const auto& linkObject : model.getLinkObjects()) {
			for (const auto& meeting : linkObject->getMeetings()) {
				if (m_objects.get(index)->getUID() == linkObject->getObject(meeting.j)->getUID()) {
					collisions += meeting.driftCollisions;
				}
			}
		}
		return collisions;
		}
	);
}



RamResultWriter::RamResultWriter(const sim::InteractionModel& model, const tdb::IndexedObjects& objects, const tdb::IndexedVertices& vertices)
	: Base(objects, vertices)
{
	addStringAttribute("type", [&](const size_t index) -> const std::string { return m_objects.get(index)->getType(); });
	addStringAttribute("descript", [&](const size_t index) -> const std::string { return m_objects.get(index)->getDescription(); });
	addDoubleAttribute("exposures", [&](const size_t index) -> const double {
		double exposure = 0.0;
		for (const auto& linkObject : model.getLinkObjects()) {
			for (const auto& meeting : linkObject->getMeetings()) {
				if (m_objects.get(index)->getUID() == linkObject->getObject(meeting.j)->getUID()) {
					exposure += meeting.ramExposure;
				}
			}
		}
		return exposure;
		}
	);
	addDoubleAttribute("events", [&](const size_t index) -> const double {
		double events = 0.0;
		for (const auto& linkObject : model.getLinkObjects()) {
			for (const auto& meeting : linkObject->getMeetings()) {
				if (m_objects.get(index)->getUID() == linkObject->getObject(meeting.j)->getUID()) {
					events += meeting.ramEvents;
				}
			}
		}
		return events;
		}
	);
	addDoubleAttribute("collisions", [&](const size_t index) -> const double {
		double collisions = 0.0;
		for (const auto& linkObject : model.getLinkObjects()) {
			for (const auto& meeting : linkObject->getMeetings()) {
				if (m_objects.get(index)->getUID() == linkObject->getObject(meeting.j)->getUID()) {
					collisions += meeting.ramCollisions;
				}
			}
		}
		return collisions;
		}
	);
}

} // namespace shapelib
} // namespace sim
