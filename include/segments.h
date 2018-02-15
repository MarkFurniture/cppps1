#ifndef __SEGMENTS_H_INCLUDED__
#define __SEGMENTS_H_INCLUDED__

#include <map>
#include <vector>
#include <libconfig.h++>

class Segments {
private:
	// member variables
	static const int FG = 1;
	static const int BG = 0;
	static const std::string angular;
	static const std::string angularThin;
	static const std::string upArrow;
	static const std::string downArrow;
	static const std::string upArrowBig; 
	static const std::string downArrowBig; 
	static const std::string lock; 
	static const std::string tick; // ✓
	static const std::string tickHeavy; // ✔
	static const std::string cross; // ✗
	static const std::string crossHeavy; // ✘
	static const std::string alt; // ⌥
	static const std::string pencil; //✐

	typedef std::string (Segments::*fnPtr)();
	std::string cfgFile;
	libconfig::Config cfg;
	std::map<std::string, fnPtr> fnMap;
	std::string status;
	bool cfgPresent;

	bool readConfig(const char* cfgFile);
	std::string colour(std::string fg, std::string bg, std::string txt);
	std::string fg(std::string fg);
	std::string bg(std::string bg);
	std::string defaultColour(int where);
	std::string resetColour();
	std::string getHomeDir();
	std::string getStr(std::string key, std::string defaultValue);
	bool executeCmd(std::string *outPtr, std::string cmd, int bufSize);
	bool isRoot();
	void funcMap();

public:
	// helper functions
	std::string callFunc(std::string fn);
	std::string endPrompt();
	std::string replaceColours(std::string ps1);

	// Add segments below
	std::string timestamp();
	std::string username();
	std::string hostname();
	std::string cwd();
	std::string git();
	std::string exit_status();
	std::string prompt();

	Segments(std::string status, std::string cfgFile);
	~Segments();
};

#endif