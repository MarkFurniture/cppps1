#ifndef __SEGMENTS_H_INCLUDED__
#define __SEGMENTS_H_INCLUDED__

#include <map>

class Segments {
private:
	// member variables
	static const int FG = 1;
	static const int BG = 0;
	static const std::string sep;
	static const std::string sepThin;

	typedef std::string (Segments::*fnPtr)();
	std::map<std::string, fnPtr> fnMap;
	std::string status;
	bool root;

	std::string colour(std::string fg, std::string bg, std::string txt);
	std::string fg(std::string fg);
	std::string bg(std::string bg);
	std::string defaultColour(int where);
	std::string resetColour();
	std::string getHomeDir();
	void funcMap();
	bool isRoot();
public:
	// helper functions
	std::string callFunc(std::string fn);
	std::string endPrompt();
	std::string replaceColours(std::string ps1);

	// Add segments below
	std::string username();
	std::string hostname();
	std::string cwd();
	std::string git();
	std::string exit_status();
	std::string prompt();

	Segments(std::string status);
	~Segments();
};

#endif