#ifndef __SEGMENTS_H_INCLUDED__
#define __SEGMENTS_H_INCLUDED__

#include <map>

#define FG 1
#define BG 0

class Segments {
private:
	// member variables
	typedef std::string (Segments::*fnPtr)();
	std::map<std::string, fnPtr> fnMap;
	std::string status;
	bool root;

	std::string colour(std::string fg, std::string bg, std::string txt);
	std::string default_colour(int where);
	std::string reset_colour();
	std::string getHomeDir();
	void funcMap();
	bool isRoot();
public:
	// helper functions
	std::string callFunc(std::string fn);
	std::string endPrompt();
	std::string replace_colours(std::string ps1);

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