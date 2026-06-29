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
#include <sem/Cell.h>
#include <sem/SubCellObject.h>
#include <tdb/TrafficDataBase.h>

namespace sem
{

struct CellMeetingData
{
	double causationFactor;						// Stores the causation factor when computing collisions
	std::vector<double> exposures;
	std::vector<double> collisions;
	std::vector<size_t> objectIndex;			// The global object index
};



class SEM_EXPORT CellObject : public sim::CellObject
{
	using base = sim::CellObject;
public:
	CellObject(const sim::Cell& cell, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase, const tdb::WeatherstationScheme& wss);

	void setup() final;

	const auto& getShieldingObjects() const { return m_shieldingObjects; }

	const auto& getNonShieldingObjects() const { return m_nonShieldingObjects; }

	const auto& getVerticesShieldingObjects() const { return m_verticesShieldingObjects; }

	const auto& getVerticesNonShieldingObjects() const { return m_verticesNonShieldingObjects; }

	bool usePassingDistance(const sim::Object* object) const;

	inline bool useAvoidance() const { return m_model.getUseAvoidance(); }

private:
	void prepareExposureComputation() final;

	void prepareEventComputation() final;

	void prepareCollisionComputation() final;

	void computeExposure(const sim::ShipObjectMeeting& meeting, double& driftExposure, double& ramExposure) final;

	void computeEvents(const sim::ShipObjectMeeting& meeting, double& driftEvents, double& ramEvents) final;

	void computeCollisions(const sim::ShipObjectMeeting& meeting, double& driftCollisions, double& ramCollisions) final;

	void projectCoordinates(const std::vector<const sim::Object*>& objectList, std::vector<std::vector<sml::Vector2d>>& verticesObjects);

	void discretise();

	const sem::Cell& m_cell;
	const SamsonExposureModel& m_model;
	const std::vector<DriftModel>& m_driftModels;

	// Objects are treated differently based on their shielding capabilities
	std::vector<const sim::Object*> m_shieldingObjects;
	std::vector<const sim::Object*> m_nonShieldingObjects;

	// The vertices of the object projected to the 2D plane of this LinkObject
	// TODO check if this really needs to be stored as member variable
	std::vector<std::vector<sml::Vector2d>> m_verticesShieldingObjects;
	std::vector<std::vector<sml::Vector2d>> m_verticesNonShieldingObjects;

	// Discretisation of the current cell
	std::vector<SubCellObject> m_subCells;
	// Subcell meetings contain exposure, event or collission data for the probability of a [shipIndex] and objectIndex
	std::vector<CellMeetingData> m_meetingsDrift;		// [shipIndex]
	std::vector<CellMeetingData> m_meetingsRam;			// [shipindex]
};
} // namespace sem

