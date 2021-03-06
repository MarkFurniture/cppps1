#include <iostream>
#include "ps1.h"

int main(int argc, char **argv)
{
	std::string cfgFile = (std::string)getenv("HOME") + "/.cppps1";

	std::string prefix = (argc < 3) ? "" : argv[2];
	std::string exitCode = (argc < 2) ? "0" : argv[1];

	PS1 p(prefix, cfgFile);
	std::string ps1 = p.generate(exitCode);
	std::cout << ps1 << std::endl;

	return 0;
}