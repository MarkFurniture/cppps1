#include <iostream>
#include <vector>
#include "ps1.h"
#include "segments.h"
#include <libconfig.h++>

PS1::PS1() : PS1::PS1("") {}

PS1::PS1(std::string prefix)
{
	this->prefix = prefix;

	// this->cfgFile = "/Users/mike/src/cpp/ps1/.cppps1";
	this->cfgFile = (std::string)getenv("HOME") + "/.cppps1";
	readConfig(this->cfgFile.c_str());
}

bool PS1::readConfig(const char* cfgFile)
{
	try {
		this->cfg.readFile(cfgFile);
		return true;
	} catch (libconfig::FileIOException &fioex) {
		std::cout << "[CPPPS1] Could not find config file" << std::endl;
	} catch (libconfig::ParseException &pex) {
		std::cout << "[CPPPS1] Error parsing config file" << std::endl;
	}

	return false;
}

std::vector<std::string> PS1::getOptions()
{
	static const std::string optArr[] = { "timestamp", "username", "hostname", "cwd", "git", "prompt" };
	std::vector<std::string> v(optArr, optArr + sizeof(optArr) / sizeof(optArr[0]) );

	return v;
}

std::string PS1::generate(std::string exitCode)
{
	Segments s(exitCode, cfg);
	std::vector<std::string> opts = this->getOptions();

	// get data
	std::string ps1 = this->prefix;
	for (auto const& opt: opts)
		ps1 += s.callFunc(opt);

	ps1 += s.endPrompt();

	return ps1;
}