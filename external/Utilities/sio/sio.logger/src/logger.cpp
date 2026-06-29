// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sio/logger/logger.h>
#include <iostream>

namespace sio {
namespace logger {

Logger::Logger()
	: m_logLevel(LogLevel::Info)
{

}



Logger::~Logger()
{ 
	m_file.close(); 
}



void Logger::setFileName(std::string_view fileName)
{
	// Attempt to create the file or clear an existing one
	m_file.open(fileName.data(), std::ios::trunc);
	if (!m_file.is_open()) {
		std::cout << fileName << " could not be created or opened" << std::endl;
	}
	m_file.close();

	// Open the file for logging
	m_file.open(fileName.data(), std::ios::app | std::ios::out);
	if (!m_file.is_open()) {
		std::cout << fileName << " could not be opened." << std::endl;
	}
}



void Logger::setLogLevel(LogLevel logLevel)
{
	if (logLevel == LogLevel::Unknown) {
		LogWarning() << "Unknown log level, setting log level to info.";
		LogWarning() << "Available log levels are: verbose, debug, info, warning, error, fatal.";
	}
	else {
		m_logLevel = logLevel;
	}
}

} // namespace logger
} // namespace sio
