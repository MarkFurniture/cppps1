#ifndef __PS1_H_INCLUDED__
#define __PS1_H_INCLUDED__

#include <vector>
#include <libconfig.h++>

class PS1 {
private:
	std::string prefix;
	std::string cfgFile;

	std::vector<std::string> getOptions();
public:
	std::string generate(std::string exitCode);
	PS1(std::string prefix, std::string cfgFile);
	PS1();
};

#endif