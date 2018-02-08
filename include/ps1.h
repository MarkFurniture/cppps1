#ifndef __PS1_H_INCLUDED__
#define __PS1_H_INCLUDED__

#include <vector>
#include <libconfig.h++>

class PS1 {
private:
	std::string prefix;
	std::string cfgFile;
	libconfig::Config cfg;

	std::vector<std::string> getOptions();
	bool readConfig(const char* cfgFile);
public:
	std::string generate(std::string exitCode);
	PS1(std::string prefix);
	PS1();
};

#endif