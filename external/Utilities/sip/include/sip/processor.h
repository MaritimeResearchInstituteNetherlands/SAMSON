// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sip_export.h>

#include <sim/InteractionModel.h>
#include <tdb/TrafficDataBase.h>
#include "yaml-cpp/yaml.h"

#include <unordered_map>
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>

namespace sip {
	
class SIP_EXPORT Processor
{
	using Action = std::function<void(std::string&, std::stringstream&)>;
public:
	Processor();

	void initialise(const std::string& config, const std::string& actions);

	void execute();

	void terminate();

private:
	void load(std::string& target, std::stringstream& input);

	void save(std::string& target, std::stringstream& input);

	void modify(std::string& target, std::stringstream& input);

	void compute(std::string& target, std::stringstream& input);

	void help(std::string& target, std::stringstream& input);

	bool validAspect(std::string& aspect);

	std::unordered_map<std::string, std::vector<std::string>> getFileLocations(std::string fileLocations);

	// Workspace configuration
	YAML::Node m_config;
	tdb::FileLocations m_fileLocations;
	std::filesystem::path m_currentDirectory;
	std::filesystem::path m_inputFolder;
	std::filesystem::path m_outputFolder;
	std::filesystem::path m_exportFolder;

	// Actions
	std::vector<std::string> m_actions;
	std::vector<std::string> m_aspects;
	std::unordered_map<std::string, Action> m_actionMap;
	
	// DataBases
	tdb::TrafficDataBase m_trafficDataBase;
	//std::map<std::string, tdb::Instance> m_instances;

	// Interaction models
	bool m_parallel;
	std::unique_ptr<sim::InteractionModel> m_interactionModel;
	//std::map<std::string, sim::InstanceModel> m_instanceModels;
};

} // namespace sip
