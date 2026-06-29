// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <sio.logger_export.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <mutex>

namespace sip { class Processor; }

namespace sio {
namespace logger {

enum class LogLevel 
{ 
	Verbose,
	Debug,
	Info,
	Action,
	Warning, 
	Error, 
	Fatal,
	Unknown
};


static std::string convertLogLevel(LogLevel logLevel)
{
	std::string level = "unknown";
	if (logLevel == LogLevel::Verbose) {
		level = "verbose";
	}
	if (logLevel == LogLevel::Debug) {
		level = "debug";
	}
	if (logLevel == LogLevel::Info) {
		level = "info";
	}
	if (logLevel == LogLevel::Action) {
		level = "action";
	}
	if (logLevel == LogLevel::Warning) {
		level = "warning";
	}
	if (logLevel == LogLevel::Error) {
		level = "error";
	}
	if (logLevel == LogLevel::Fatal) {
		level = "fatal";
	}
	return level;
}



static LogLevel convertLogLevel(std::string_view level) 
{
	LogLevel logLevel = LogLevel::Unknown;
	if (level == "verbose") {
		logLevel = LogLevel::Verbose;
	}
	if (level == "debug") {
		logLevel = LogLevel::Debug;
	}
	if (level == "info") {
		logLevel = LogLevel::Info;
	}
	if (level == "action") {
		logLevel = LogLevel::Action;
	}
	if (level == "warning") {
		logLevel = LogLevel::Warning;
	}
	if (level == "error") {
		logLevel = LogLevel::Error;
	}
	if (level == "fatal") {
		logLevel = LogLevel::Fatal;
	}

	return logLevel;
}

class SIO_LOGGER_EXPORT Logger
{
public:
	static inline Logger& get() 
	{ 
		static Logger instance;
		return instance;
	}

	void setFileName(std::string_view fileName);

	void setLogLevel(LogLevel logLevel);

	inline LogLevel getLogLevel() const { return m_logLevel; }

	inline void printVerbose(const std::ostringstream& message) { if (LogLevel::Verbose >= m_logLevel) { print("VERBOSE ", message); } }

	inline void printDebug(const std::ostringstream& message) { if (LogLevel::Debug >= m_logLevel) { print("DEBUG ", message); } }

	inline void printInfo(const std::ostringstream& message) { if (LogLevel::Info >= m_logLevel) { print("INFO ", message); } }

	inline void printAction(const std::ostringstream& message) { if (LogLevel::Info >= m_logLevel) { print("ACTION ", message); } }

	inline void printWarning(const std::ostringstream& message) { if (LogLevel::Warning >= m_logLevel) { print("WARNING ", message); } }

	inline void printError(const std::ostringstream& message) { if (LogLevel::Error >= m_logLevel) { print("ERROR ", message); } }

	inline void printFatal(const std::ostringstream& message) { if (LogLevel::Fatal >= m_logLevel) { print("FATAL ", message); } }	

	void flush() { m_file.flush(); }
	
	void close() { m_file.close(); }

private:
	Logger();

	~Logger();

	inline void print(std::string_view logLevel, const std::ostringstream& message) { std::lock_guard<std::mutex> lock(m_mutex); m_file << logLevel << message.str() << std::endl; }

	LogLevel m_logLevel;
	std::ofstream m_file;
	std::mutex m_mutex;
};



class BaseLogType
{
public:
	BaseLogType() { m_stream << std::setprecision(8); }

	template <class T>
	inline BaseLogType& operator<<(T s) { m_stream << s; return *this; }

protected:
	std::ostringstream m_stream;
};



class SIO_LOGGER_EXPORT LogVerbose : public BaseLogType
{
public:
	inline ~LogVerbose()	{ Logger::get().printVerbose(m_stream); }
};



class SIO_LOGGER_EXPORT LogDebug : public BaseLogType
{
public:
	inline ~LogDebug() { Logger::get().printDebug(m_stream); }
};



class SIO_LOGGER_EXPORT LogInfo : public BaseLogType
{
public:
	inline ~LogInfo() { Logger::get().printInfo(m_stream); }
};



class SIO_LOGGER_EXPORT LogAction : public BaseLogType
{
	inline ~LogAction() { Logger::get().printAction(m_stream); }

private:
	LogAction() {}
	friend class sip::Processor;	// Only the processor may call an action
};



class SIO_LOGGER_EXPORT LogWarning : public BaseLogType
{
public:
	inline ~LogWarning() { Logger::get().printWarning(m_stream); }
};



class SIO_LOGGER_EXPORT LogError : public BaseLogType
{
public:
	inline ~LogError() { Logger::get().printError(m_stream); }
};



class SIO_LOGGER_EXPORT LogFatal : public BaseLogType
{
public:
	inline ~LogFatal() { 
		Logger::get().printFatal(m_stream); 
		Logger::get().close();
	}
};

} // namespace logger 
} // namespace sio
