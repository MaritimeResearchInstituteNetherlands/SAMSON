// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include "sim.csv_export.h"
#include <sim/InteractionModel.h>
#include <sim/csv/OutputSchemes.h>
#include <sim/Link.h>
#include <tdb/Model.h> // TODO move to separate util library
#include "util/IModel.h"
#include <string>

namespace sim
{
namespace csv
{

class SIM_CSV_EXPORT LoaderSaver : public tdb::BaseModel // TODO move to separate samson database library sdl
{
public:
	LoaderSaver(const InteractionModel& interactionModel, std::string directory);

	void save();

	void load();

private:
	bool saveLink();

	bool saveCell();

	bool saveLinkLink();

	bool saveLinkCell();

	bool saveLinkObject();

	bool saveCellObject();

	bool loadLink();

	bool loadCell();

	bool loadLinkLink();

	bool loadLinkCell();

	bool loadLinkObject();

	bool loadCellObject();

	const InteractionModel& m_interactionModel;
	std::string m_directory; 
};

} // namespace csv
} // namespace sim
