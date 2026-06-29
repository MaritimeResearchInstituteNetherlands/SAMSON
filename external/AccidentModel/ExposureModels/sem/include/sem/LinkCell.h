// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sem_export.h"
#include <sem/ExposureModel.h>
#include <sem/Link.h>
#include <sem/Cell.h>
#include <sim/InteractionTypes.h>

namespace sem
{
class SEM_EXPORT LinkCell : public sim::LinkCell
{
	using base = sim::LinkCell;

	struct SubCell
	{
		double area;
		double offset0;
		double offset1;
	};

	struct SubCellExposure
	{
		double exposure;
		double struckChance_i;
		double struckChance_j;
	};

public:
	LinkCell(const sim::Link& link, const sim::Cell& cell, const SamsonExposureModel& model, const tdb::TrafficDataBase& trafficDatabase);

	void setup() final;

private:
	void computeExposure(const sim::ShipShipMeeting& meeting, double& exposure, double& struckChance_i, double& struckChance_j) final;

	void computeEvent(const sim::ShipShipMeeting& meeting, double& events_i, double& events_j) final;

	void computeCollision(const sim::ShipShipMeeting& meeting, double& collisions_i, double& collisions_j) final;

	// References to data
	const Link& m_link;								// Reference to linkA
	const Cell& m_cell;								// Reference to linkB
	const SamsonExposureModel& m_model;				// Reference to the data, causation model
	const tdb::TrafficDataBase& m_trafficDatabase;	// Reference to the traffic database

	double m_angle;									// Angle of the link
	double m_totalArea;								// Total area
	double m_dwidth;								// width of a sublink
	std::vector<SubCell> m_subCells;				// Subcells where the link is assumed to have a constant probability
};
} // namespace sem

