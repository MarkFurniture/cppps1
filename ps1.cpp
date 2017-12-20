#include <iostream>
#include <vector>
#include "segments.h"

class PS1 {
private:
	std::string prefix;
	std::vector<std::string> getOptions();
public:
	std::string generate(std::string exitCode);
	PS1(std::string prefix);
	PS1();
};

PS1::PS1() : PS1::PS1("") {}

PS1::PS1(std::string prefix)
{
	this->prefix = prefix;
}

std::vector<std::string> PS1::getOptions()
{
	static const std::string optArr[] = { "username", "hostname", "cwd", "git", "prompt" };
	std::vector<std::string> v (optArr, optArr + sizeof(optArr) / sizeof(optArr[0]) );

	return v;
}

std::string PS1::generate(std::string exitCode)
{	
	Segments::Segments s(exitCode);
	std::vector<std::string> opts = this->getOptions();

	// get data
	std::string unformatted = this->prefix;
	for (auto const& opt: opts)
		unformatted += s.callFunc(opt);

	unformatted += s.endPrompt();

	std::string formatted = s.replace_colours(unformatted);

	return formatted;
}


int main(int argc, char **argv)
{
	std::string prefix = (argc < 3) ? "" : argv[2];
	std::string exitCode = (argc < 2) ? "0" : argv[1];
	
	// PS1 p("🌀  ");
	PS1 p(prefix);
	std::string ps1 = p.generate(exitCode);
	std::cout << ps1 << std::endl;

	return 0;
}