#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unistd.h>
#include <sys/stat.h>
#include <map>
#include "segments.h"

Segments::Segments(std::string status)
{
	this->status = status;
	this->root = this->isRoot();
	
	// set up fn name -> fn pointer mapping
	this->funcMap();
}

Segments::~Segments()
{}

std::string Segments::colour(std::string fg, std::string bg, std::string txt)
{
	return "\\[\\e[38;5;" + fg + "m\\]\\[\\e[48;5;" + bg + "m\\]" + txt + "\\[\\e[48;5;" + bg + "m\\]\\[\\e[38;5;" + fg + "m\\]";
}

std::string Segments::default_colour(int where)
{
	return (where ? "\\[\\e[39m" : "\\[\\e[49m");
}

std::string Segments::reset_colour()
{
	return "\\[\\e[0m\\]";
}

std::string Segments::replace_colours(std::string ps1)
{
	std::regex fg("\\{f(\\d+)\\}");
	std::regex bg("\\{b(\\d+)\\}");
	std::string ps1Replaced = std::regex_replace(ps1, fg, "\\[\\e[38;5;$1m\\]");
	ps1Replaced = std::regex_replace(ps1Replaced, bg, "\\[\\e[48;5;$1m\\]");

	return ps1Replaced;
}

std::string Segments::endPrompt()
{
	return this->default_colour(0) + "" + this->reset_colour() + " ";
}

std::string Segments::getHomeDir()
{
	return getenv("HOME");
}

bool Segments::isRoot()
{
	return !strcmp((const char*)getenv("USER"), "root");
}

std::string Segments::callFunc(std::string fn)
{
	return (this->*fnMap[fn])();
}

//*****************************************************//
//				  Add segments below				   //
//*****************************************************//

void Segments::funcMap()
{
	this->fnMap["username"] = &Segments::username;
	this->fnMap["hostname"] = &Segments::hostname;
	this->fnMap["cwd"] = &Segments::cwd;
	this->fnMap["git"] = &Segments::git;
	this->fnMap["exit_status"] = &Segments::exit_status;
	this->fnMap["prompt"] = &Segments::prompt;
}

std::string Segments::username()
{
	std::string username = getenv("USER");
	std::string fg = "46", bg = "240";

	if (this->root) {
		fg = "255";
		bg = "204";
	}

	return "{f" + fg + "}{b" + bg + "} \\u {f" + bg + "}";
}

std::string Segments::hostname()
{
	char *buffer;
	int bufSize = 100;
	buffer = (char*) malloc(bufSize);
	gethostname(buffer, bufSize);
	std::string hostname(buffer);
	free(buffer);

	// colour based on user
	std::string fg = "46", bg = "238";

	if (this->root) {
		fg = "255";
		bg = "204";
	}

	return "{f" + fg + "}{b" + bg + "} \\h {f" + bg + "}";
}

std::string Segments::cwd()
{
	// get cwd
	char *buffer; int bufSize = 1000;
	buffer = (char*) malloc(bufSize);
	getcwd(buffer, bufSize);
	std::string cwd(buffer);
	free(buffer);

	// replace home dir path with ~
	std::regex re("^" + this->getHomeDir());
	std::string cwdReplaced = std::regex_replace(cwd, re, "~");

	return "{b31}{f255} " + cwdReplaced + " {f31}";
}

std::string Segments::git()
{
	struct stat st;
	std::string gitStr = "";

	if (stat(".git", &st) == 0 && S_ISDIR(st.st_mode)) {
		FILE *gitBranch_p = popen("git rev-parse --abbrev-ref HEAD 2> /dev/null", "r");

		if (gitBranch_p) {
			char *buffer; int bufSize = 100;
			buffer = (char*) malloc(bufSize);
			fgets(buffer, sizeof(buffer), gitBranch_p);
			pclose(gitBranch_p);
			gitStr = (std::string)buffer;
			gitStr.erase(std::remove(gitStr.begin(), gitStr.end(), '\n'), gitStr.end());
		}
	}

	return gitStr.length() ? "{b220}{f0} " + gitStr + " {f220}" : "";
}

std::string Segments::exit_status()
{
	return "" + this->status;
}

std::string Segments::prompt()
{
	std::string un = getenv("USER");
	std::string prompt = (this->root ? "#" : "$");
	std::string col = (this->status == "0" ? "240" : "204");

	return "{b" + col + "}{f255} " + prompt + " {f" + col + "}";
}






