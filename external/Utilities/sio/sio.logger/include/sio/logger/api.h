// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sio/logger/logger.h>

namespace sio {
namespace logger {

#define Log(t,m) t() << m;
#define LogVerbose(m) Log(sio::logger::LogVerbose,m)
#define LogDebug(m) Log(sio::logger::LogDebug,m)
#define LogInfo(m) Log(sio::logger::LogInfo,m)
#define LogAction(m) Log(sio::logger::LogAction,m)
#define LogWarning(m) Log(sio::logger::LogWarning,m)
#define LogError(m) Log(sio::logger::LogError,m)
#define LogFatal(m) Log(sio::logger::LogFatal,m); std::exit(-1);

static void initialise(std::string_view fileName, LogLevel logLevel)
{
	Logger::get().setFileName(fileName);
	Logger::get().setLogLevel(logLevel);

	LogInfo("Logfile location: " << fileName);
	LogInfo("Log level: " << convertLogLevel(logLevel));
}



static void initialise(std::string_view fileName, std::string_view level)
{
	initialise(fileName, convertLogLevel(level));
}

} // namespace logger 
} // namespace sio
