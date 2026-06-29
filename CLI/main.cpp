// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#define NOMINMAX
#include <windows.h>
#include <debugapi.h>
#include <sip/processor.h>

void displayHelp()
{
    std::cout << "Available arguments:\n";
    std::cout << "--config-yaml <value>    : Configuration yaml file\n";
	std::cout << "--actions <value>        : Action file\n";
    std::cout << "--help                   : Display this help message\n";
}



void FatalError(std::string msg)
{
	std::cout << msg << std::endl;
	std::exit(-1);
}



int main(int argc, char *argv[])
{
    if (argc == 1) {
        displayHelp();
        return 0;
	}

	std::unordered_map<std::string, std::string> arguments;
	for (int i = 1; i < argc; i += 2)
	{
		std::string name = argv[i];

		// Check if the argument is the help command
		if (name == "--help")
		{
			displayHelp();
			exit(0);
		}

		if (i + 1 < argc)
		{
			std::string value = argv[i + 1];
			if (name[0] == '-' && name[1] == '-')
			{
				name = name.substr(2); // Remove the leading '--'
				arguments[name] = value;
			}
			else
			{
				std::cerr << "Invalid argument format: " << name << std::endl;
				exit(1);
			}
		}
		else
		{
			std::cerr << "Missing value for argument: " << argv[i] << std::endl;
			exit(1);
		}
	}

	if (arguments.find("wait-for-debugger") != arguments.end()) {
		while (!::IsDebuggerPresent())
			Sleep(100);
	}

	sip::Processor processor;


	if (arguments.count("config-yaml") == 0) {
		std::cout << "Error: argument --config-yaml not specified." << std::endl;
		std::exit(-1);
	}
	if (arguments.count("actions") == 0) {
		std::cout << "Error: argument --actions not specified." << std::endl;
		std::exit(-1);
	}
	processor.initialise(arguments["config-yaml"], arguments["actions"]);

	processor.execute();

    return 0;
}