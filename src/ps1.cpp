#include <vector>
#include "ps1.h"
#include "segments.h"

PS1::PS1() : PS1::PS1("", "") {}

PS1::PS1(std::string prefix, std::string cfgFile)
{
	this->prefix = prefix;
	this->cfgFile = cfgFile;
}

std::vector<std::string> PS1::getOptions()
{
	static const std::string optArr[] = {"timestamp", "username", "hostname", "venv", "cwd", "git", "prompt"};
	std::vector<std::string> v(optArr, optArr + sizeof(optArr) / sizeof(optArr[0]));

	return v;
}

std::string PS1::generate(std::string exitCode)
{
	Segments s(exitCode, cfgFile);
	std::vector<std::string> opts = this->getOptions();

	// get data
	std::string ps1 = this->prefix;
	for (auto const &opt : opts)
		ps1 += s.callFunc(opt);

	ps1 += s.endPrompt();

	return ps1;
}