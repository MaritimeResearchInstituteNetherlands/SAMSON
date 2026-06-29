// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sem_export.h"
#include <sim/InteractionTypes.h>
#include <sem/ExposureModel.h>
#include <sem/Link.h>
#include <sem/SubLinkObject.h>
#include <sml/AngleBounds.h>
#include <vector>

namespace sem
{

struct LinkObjectMeetingData
{
	double causationFactor;
	std::vector<double> exposures;
	std::vector<double> collisions;
	std::vector<size_t> globalObjectIndex;
};

class SEM_EXPORT LinkObject : public sim::LinkObject
{
	using base = sim::LinkObject;
public:
	LinkObject(const sim::Link& link, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase, const tdb::WeatherstationScheme& wss);

	void setup() final;

	const auto& getShieldingObjects() const { return m_shieldingObjects; }

	const auto& getNonShieldingObjects() const { return m_nonShieldingObjects; }

	const auto& getVerticesShieldingObjects() const { return m_verticesShieldingObjects; }

	const auto& getVerticesNonShieldingObjects() const { return m_verticesNonShieldingObjects; }

	const std::vector<double>& getDriftDiscretisation() const { return m_driftDiscretisation; }

	const std::vector<double>& getRamDiscretisation() const { return m_ramDiscretisation; }

	const double getLinkAngle() const { return m_linkAngle; }

	bool usePassingDistance(const sim::Object* object) const;

	inline bool useAvoidance() const { return m_model.getUseAvoidance(); }

private:
	void prepareExposureComputation() final;

	void prepareEventComputation() final;

	void prepareCollisionComputation() final;

	void computeExposure(const sim::ShipObjectMeeting& meeting, double& driftExposure, double& ramExposure) final;

	void computeEvents(const sim::ShipObjectMeeting& meeting, double& driftEvents, double& ramEvents) final;

	void computeCollisions(const sim::ShipObjectMeeting& meeting, double& driftCollisions, double& ramCollisions) final;

	void projectCoordinates(const std::vector<const sim::Object*>& objectList, std::vector < std::vector<sml::Vector2d>>& verticesObjects);

	// Constant data references
	const tdb::TrafficDataBase& m_trafficDataBase;
	const SamsonExposureModel& m_model;
	const sem::Link& m_link;
	const std::vector<DriftModel>& m_driftModels;

	// 
	double m_linkAngle;
	std::vector<double> m_ramDiscretisation;
	std::vector<double> m_driftDiscretisation;

	//
	std::vector<LinkObjectMeetingData> m_meetingsDrift;
	std::vector<LinkObjectMeetingData> m_meetingsRam;

	// Objects are treated differently based on their shielding capabilities
	std::vector<const sim::Object*> m_shieldingObjects;
	std::vector<const sim::Object*> m_nonShieldingObjects;

	// The vertices of the object projected to the 2D plane of this LinkObject
	// TODO check if this really needs to be stored as member variable
	std::vector<std::vector<sml::Vector2d>> m_verticesShieldingObjects;
	std::vector<std::vector<sml::Vector2d>> m_verticesNonShieldingObjects;

	// To facilitate the numerical integration of the distribution on a link, a sub link is introduced which holds sub-link specific data
	std::vector<SubLinkObject> m_subLinks;		
	double m_subLinkWidth;	// Width of the sublink bin

};
} // namespace sem



