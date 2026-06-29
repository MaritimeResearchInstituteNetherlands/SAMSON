// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sip/processor.h>
#include <sim/shapelib/writer.h>
#include <sim/csv/LoaderSaver.h>
#include <sem/ExposureModel.h>
#include <tdb/shapelib/writer.h>
#include <sio/logger/api.h>

#include <iostream>
#include <memory>
#include <algorithm>

namespace sip {

Processor::Processor()
	: m_parallel(false)
{
	m_actionMap["load"] = [this](std::string& target, std::stringstream& input) { this->load(target, input); };
	m_actionMap["save"] = [this](std::string& target, std::stringstream & input) {this->save(target, input); };
	m_actionMap["modify"] = [this](std::string& target, std::stringstream& input) {this->modify(target, input); };
	//m_actionMap["apply"] = [this](std::string& target, std::stringstream& input) {this->apply(target, input); } // Applies modifications?
	m_actionMap["compute"] = [this](std::string& target, std::stringstream& input) { this->compute(target, input); };
	m_actionMap["help"] = [this](std::string& target, std::stringstream& input) { this->help(target, input); };

	m_aspects.push_back("area");
	m_aspects.push_back("traffic");
	m_aspects.push_back("exposure");
	m_aspects.push_back("event");
	m_aspects.push_back("collision");
	m_aspects.push_back("results");
	// TODO rethink if we need to refactor the action method for improved usability and clearer code
}



void Processor::initialise(const std::string& config, const std::string& actions)
{
	// Load configuration
	m_currentDirectory = std::filesystem::current_path();

	// Load config
	try {
		m_config = YAML::LoadFile(config);
	}
	catch (const YAML::ParserException& e) {
		std::cerr << "Error parsing YAML: " << e.what() << std::endl;
	}

	// Initialise logger
	std::string logLevel = "info";
	std::string logFileName = "samson.log";
	auto loggerNode = m_config["logger"];
	if (loggerNode) {
		if (loggerNode["fileName"]) {
			logFileName = loggerNode["fileName"].as<std::string>();
		}
		if (loggerNode["logLevel"]) {
			logLevel = loggerNode["logLevel"].as<std::string>();
		}
	}
	std::string logFile = m_currentDirectory.string() + "\\" + logFileName;
	sio::logger::initialise(logFile, logLevel);

	// Check api version
	if (YAML::Node parameter = m_config["apiVersion"]) {
		std::string inputApiVersion = parameter.as<std::string>();
		if (!((inputApiVersion.compare("0.0.2")) == 0)) {
			LogError("Expecting an apiVersion of 0.0.2");
		}
	}
	else {
		LogError("Api version unknown.");
	}

	// Initialise the input and output paths
	if (YAML::Node parameter = m_config["inputLocation"]) {
		m_inputFolder = m_currentDirectory.string() + parameter.as<std::string>();
	} else {
		LogError("inputLocation not given in the configuration file.");
	}

	if (YAML::Node parameter = m_config["outputLocation"]) {
		m_outputFolder = m_currentDirectory.string() + parameter.as<std::string>();
	} else {
		m_outputFolder = m_currentDirectory.string() + "\\Results\\";
		LogWarning("outputLocation not set in the configuration. Setting output folder to " << m_outputFolder);
	}

	if (YAML::Node parameter = m_config["exportLocation"]) {
		m_exportFolder = m_currentDirectory.string() + parameter.as<std::string>();
	}
	else {
		m_exportFolder = m_currentDirectory.string() + "\\Exports\\";
		LogWarning("exportLocation not set in the configuration. Setting output folder to " << m_exportFolder);
	}

	if (YAML::Node parameter = m_config["parallel"]) {
		m_parallel = parameter.as<bool>();
	}

	LogInfo("Current working directory: " << m_currentDirectory.string());
	LogInfo("Input database location: " << m_inputFolder.string());
	LogInfo("Output database location: " << m_outputFolder.string());
	LogInfo("Export location: " << m_exportFolder.string());

	// Create output folders
	std::error_code errCode;
	if (!std::filesystem::exists(m_inputFolder, errCode)) {
		LogFatal("Input folder " << m_inputFolder.string() << " does not exist.");
		terminate();
	}
	LogInfo("Input folder found.");

	std::filesystem::create_directory(m_outputFolder, errCode);
	if (errCode.value() != 0) {
		LogFatal("Output folder could not be created. Error code: " << errCode.message());
		terminate();
	}
	else {
		LogInfo("Output Result directory created.");
	}

	std::filesystem::create_directory(m_exportFolder, errCode);
	if (errCode.value() != 0) {
		LogFatal("Export folder could not be created. Error code: " << errCode.message());
		terminate();
	}
	else {
		LogInfo("Export directory created.");
	}

	m_fileLocations["areaModel"] = getFileLocations("areaModel");
	m_fileLocations["trafficModel"] = getFileLocations("trafficModel");
	m_fileLocations["interactionModel"] = getFileLocations("interactionModel");

	// Load actions
	std::string actionFileName = m_currentDirectory.string() + "\\" + actions;
	LogInfo("Loading actions from " << actionFileName);
	std::ifstream stream;
	stream.open(actionFileName.c_str());
	if (stream.fail()) {
		LogWarning("Failed opening the action file.");
	}
	std::string line;
	while (std::getline(stream, line)) {
		std::stringstream stream(line);
		std::string actionType;
		stream >> actionType;
		if (actionType.empty()) {
			continue;
		}
		else if (actionType.at(0) == '#') {
			continue;
		}
		m_actions.push_back(line);
	}
}


// Input format is
// [action] [target] [aspect] [command] [arguments]
// The actions are generic top level processor commands
// The target describes which database or instance the action is applied to
// The aspect describes which part of the database the action is applied to, area/traffic/model/exposure/causation/measures
// The command describes what to do in this aspect 
// The arguments are the necessary input to perform the command
// Everything starting with # is a comment
// The processor handles the generic parts action target and aspect,
// Then the specific command is handled by specialised aspect classes
// TODO perhaps swap the action/command naming?
void Processor::execute()
{
	for (auto action : m_actions) {
		LogAction(action);
		std::stringstream stream(action);

		// Extract type of action and check if it exists
		std::string actionType;
		stream >> actionType;
		if (m_actionMap.find(actionType) == m_actionMap.end()) {
			LogWarning("Can't find action " << actionType);
			std::string actionList;
			for (auto entry : m_actionMap) {
				actionList += entry.first + " ";
			}
			LogWarning("Valid actions: " << actionList);
			continue;
		}



		// Extract target and check if it exists
		std::string target;
		stream >> target;
		if (actionType == "help") {
		} else if (target != "TrafficDataBase") {
			// For now only TrafficDataBase, in future named instances, perhaps "all" to load everything in the config file
			LogWarning("Invalid target: " << target);
			LogWarning("Valid targets: " << "TrafficDataBase");
			continue;
		}

		// Perform action
		m_actionMap[actionType](target, stream);
	}
}



void Processor::load(std::string& target, std::stringstream& input)
{
	if (target == "TrafficDataBase") {
		LogInfo("Loading TrafficDataBase");
		// For now there is only the SamsonExposureModel, but could also be another model in fture
		std::unique_ptr<tdb::BaseModel> exposureModel = std::make_unique<sem::SamsonExposureModel>();
		m_trafficDataBase.setExposureModel(exposureModel);
		m_trafficDataBase.load(m_inputFolder.string(), m_fileLocations);
	
		m_interactionModel = std::make_unique<sim::InteractionModel>(m_trafficDataBase);
		m_interactionModel->setParallel(m_parallel);
	}
	else {
		LogWarning("Unexpected error during loading");
	}
}



void Processor::modify(std::string& target, std::stringstream& input)
{
	// Get aspect
	std::string aspect;
	input >> aspect;
	if (!validAspect(aspect)) {
		return;
	}

	// Call underlying command
	if (aspect == "area") {
		m_trafficDataBase.getAreaManager().command(input);
	}
	else {
		LogWarning("Aspect " << aspect << " is not yet implemented.");
	}
}



void Processor::save(std::string& target, std::stringstream& input)
{
	// save [target] [area/traffic/exposure/event/measure] [overwrite/csv/shapefile] ]

	std::string aspect;
	input >> aspect;
	std::string type;
	input >> type;
	if (!validAspect(aspect)) {
		return;
	}

	if (aspect == "area") {
		m_trafficDataBase.getAreaManager().apply(); // Apply modification
		// TODO area somehow print the data?
		if (type == "shapefile") {
			LogInfo("saving area of " << target << " as " << type);
			// Write the input data
			tdb::shapelib::Writer shapelibWriter(m_exportFolder, m_trafficDataBase.getAreaModel());
			// Write the output data
			sim::shapelib::AreaWriter areaWriter(m_exportFolder, m_interactionModel->getLinks());
		}
		else {
			LogWarning("Unsported file type " << type);
		}
	}
	else if (aspect == "traffic") {
		LogWarning("Not yet implemented ");
	}
	else if (aspect == "results") {
		if (type == "csv") {
			LogInfo("saving results of " << target << " as " << type);
			if (!m_interactionModel) {
				LogError("Target " << target << " has not been loaded.");
			}
			else {
				sim::csv::LoaderSaver loaderSaver(*m_interactionModel.get(), m_outputFolder.string());
				loaderSaver.save();
			}
		}
		else if (type == "shapefile") {
			sim::shapelib::ResultWriter resultsWriter(m_exportFolder, m_trafficDataBase.getAreaModel(), *m_interactionModel.get());
		}
		else {
			LogWarning("Unsuported file type " << type);
		}
	}
}



void Processor::compute(std::string& target, std::stringstream& input)
{
	std::string type;
	input >> type;
	if (target == "TrafficDataBase" && type == "exposure") {
		LogInfo("Computing exposures of " << target);
		m_interactionModel->computeExposures();
	}
	else if (target == "TrafficDataBase" && type == "event") {
		LogInfo("Computing events of " << target);
		m_interactionModel->computeEvents();
	}
	else if (target == "TrafficDataBase" && type == "collision") {
		LogInfo("Computing collisions of " << target);
		m_interactionModel->computeCollisions();
	}
	else {
		LogWarning("Type " << type << " is not supported");
	}
}



void Processor::help(std::string& target, std::stringstream& input)
{
	if (target == "") {
		LogInfo("Use \"help <type>\" for more information on the input with type [areaModel, trafficModel, interactionModel]");
	}
	else if (target == "areaModel") {
		tdb::AreaModel model;
		model.help();
	}
	else if (target == "trafficModel") {
		tdb::TrafficModel model;
		model.help();
	}
	else if (target == "interactionModel") {
		if (m_config["interactionModel"]["model"].as<std::string>() == "sem") {
			sem::SamsonExposureModel model;
			model.help();
		}
	}
	else {
		LogWarning("Unkonwn help type " << target);
	}
}



bool Processor::validAspect(std::string& aspect)
{
	// Check if aspect exists
	if (std::find(m_aspects.begin(), m_aspects.end(), aspect) == m_aspects.end())
	{
		LogWarning ("Aspect " << aspect << " is not supported.");
		std::string aspects;
		for (auto item : m_aspects) {
			aspects += item + " ";
		}
		LogWarning("Supported aspects: " << aspects);
		return false;
	}
	return true;
}



std::unordered_map<std::string, std::vector<std::string>> Processor::getFileLocations(std::string locationName)
{
	// Extract file locations
	std::unordered_map<std::string, std::vector<std::string>> fileLocations;
	auto nodeMap = m_config[locationName].as<std::unordered_map<std::string, YAML::Node>>();
	for (auto& entry : nodeMap) {
		if (entry.second.IsScalar()) {
			fileLocations[entry.first].push_back(entry.second.as<std::string>());
		}
		else if (entry.second.IsSequence()) {
			for (size_t i = 0; i < entry.second.size(); i++) {
				fileLocations[entry.first].push_back(entry.second[i].as<std::string>());
			}
		}
	}
	return fileLocations;
}



void Processor::terminate()
{
	// Flush logger and exit
	sio::logger::Logger::get().flush();
	sio::logger::Logger::get().close();
	std::exit(-1);
}
} // namespace sip
