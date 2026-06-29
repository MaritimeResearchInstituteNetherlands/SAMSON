// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <tdb/Model.h> // TODO does this Model have to be in the TDB? I don't think so
#include <sem/CausationSchemes.h>

namespace sem
{
using CausationFactors = tdb::DataTable<CausationFactorsScheme>;
using IndexedCausationFactors = tdb::IndexedDataTable<CausationFactorsScheme, std::string>;

class CausationModel : public tdb::BaseModel
{
public:

	CausationModel() {}

	bool loadModel(const std::string& path, std::unordered_map<std::string, std::vector<std::string>>& filenames) final;

	void help() final;

	bool validate();

	inline const IndexedCausationFactors& getCausationFactors() const { return m_causationFactors; }

private:
	CausationFactors m_causationFactorsDataBase;
	IndexedCausationFactors m_causationFactors;
};
} // namespace sem

