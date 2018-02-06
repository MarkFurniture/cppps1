#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unistd.h>
#include <sys/stat.h>
#include <map>
#include <chrono>
#include <ctime>
#include "segments.h"

const std::string Segments::sep = ""; // \uE0B0
const std::string Segments::sepThin = ""; // \uE0B1
const std::string Segments::upArrow = "↑";
const std::string Segments::downArrow = "↓";
const std::string Segments::upArrowBig = "⬆";
const std::string Segments::downArrowBig = "⬇";
const std::string Segments::tick = "✓";
const std::string Segments::tickHeavy = "✔";
const std::string Segments::cross = "✗";
const std::string Segments::crossHeavy = "✘";
const std::string Segments::lock = "";
const std::string Segments::alt = "⌥";
const std::string Segments::pencil = "✐";

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

std::string Segments::fg(std::string fg)
{
	return "\\[\\e[38;5;" + fg + "m\\]";
}

std::string Segments::bg(std::string bg)
{
	return "\\[\\e[48;5;" + bg + "m\\]";
}

std::string Segments::defaultColour(int where)
{
	return (where ? "\\[\\e[39m" : "\\[\\e[49m");
}

std::string Segments::resetColour()
{
	return "\\[\\e[0m\\]";
}

std::string Segments::replaceColours(std::string ps1)
{
	std::regex fg("\\{f(\\d+)\\}");
	std::regex bg("\\{b(\\d+)\\}");
	std::string ps1Replaced = std::regex_replace(ps1, fg, "\\[\\e[38;5;$1m\\]");
	ps1Replaced = std::regex_replace(ps1Replaced, bg, "\\[\\e[48;5;$1m\\]");

	return ps1Replaced;
}

std::string Segments::endPrompt()
{
	return this->defaultColour(0) + this->sep + this->resetColour() + " ";
}

std::string Segments::getHomeDir()
{
	return getenv("HOME");
}

bool Segments::executeCmd(std::string *outPtr, std::string cmd, int bufSize)
{
	FILE *cmd_p = popen(cmd.c_str(), "r");

	if (!cmd_p)
		return false;

	char *buf;
	buf = (char*)malloc(bufSize);
	fgets(buf, sizeof(buf)*bufSize, cmd_p);
	pclose(cmd_p);

	*outPtr = (std::string)buf;
	outPtr->erase(std::remove(outPtr->begin(), outPtr->end(), '\n'), outPtr->end());

	free(buf);

	return true;
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
	this->fnMap["timestamp"] = &Segments::timestamp;
	this->fnMap["username"] = &Segments::username;
	this->fnMap["hostname"] = &Segments::hostname;
	this->fnMap["cwd"] = &Segments::cwd;
	this->fnMap["git"] = &Segments::git;
	this->fnMap["exit_status"] = &Segments::exit_status;
	this->fnMap["prompt"] = &Segments::prompt;
}

std::string Segments::timestamp()
{
	char buf[12];
	auto time = std::chrono::system_clock::now();
	std::time_t hTime = std::chrono::system_clock::to_time_t(time);
	strftime(buf, sizeof(buf), " [%I:%M:%S]", localtime(&hTime));

	// std::string fg = "46", bg = "238";
	std::string fg = "255", bg = "238";

	return this->fg(fg) + this->bg(bg) + std::string(buf) + " ";
}

std::string Segments::username()
{
	std::string username = getenv("USER");
	std::string fg = "46", bg = "240";

	if (this->root) {
		fg = "255";
		bg = "204";
	}

	return this->fg(fg) + this->bg(bg) + " \\u " + this->fg(bg);
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

	return this->fg(fg) + this->bg(bg) + " \\h " + this->fg(bg);
}

std::string Segments::cwd()
{
	// get cwd
	char *buffer; int bufSize = 1000;
	buffer = (char*) malloc(bufSize);
	getcwd(buffer, bufSize);
	std::string cwd(buffer);
	free(buffer);

	bool writable = access(cwd.c_str(), W_OK) == 0;
	std::string col = writable ? "69" : "62";

	// replace home dir path with ~
	std::regex re("^" + this->getHomeDir());
	std::string cwdReplaced = std::regex_replace(cwd, re, "~");

	// replace slashes with chevrons
	std::regex slashes("\\/");
	std::string cwdSeparated = std::regex_replace(cwdReplaced, slashes, this->fg("240") + " " + this->sepThin + " " + this->fg("255"));

	if (cwdReplaced[0] == '/')
		cwdSeparated = "/" + cwdSeparated;

	return this->bg(col) + this->sep + this->fg("255") + " " + cwdSeparated + " " + this->fg(col);
}

std::string Segments::git()
{
	struct stat st;
	std::string gitStr = "";

	if (stat(".git", &st) == 0 && S_ISDIR(st.st_mode)) {
		// get names of local and remote branches
		std::string delim = ",";
		std::string localCmd = "git rev-parse --abbrev-ref HEAD 2> /dev/null";
		std::string remoteCmd = "git rev-parse --abbrev-ref HEAD@{u} 2> /dev/null";

		std::string local, remote;
		if (executeCmd(&local, localCmd, 100)) {
			// create text for git string
			gitStr = (std::string)local;

			if (executeCmd(&remote, remoteCmd, 100)) {
				// get commit counts
				std::string statusCmd = "git rev-list --left-right --count " + local + "..." + remote;
				std::string statusStr, ahead, behind;

				if (executeCmd(&statusStr, statusCmd, 20)) {
					// split commit counts into ahead/behind
					ahead = statusStr.substr(0, statusStr.find("\t"));
					statusStr.erase(0, statusStr.find("\t") + 1);
					behind = statusStr;
				} else {
					ahead = "0";
					behind = "0";
				}
				
				if (ahead.compare("0"))
					gitStr += " " + ahead + this->upArrow;
				if (behind.compare("0"))
					gitStr += " " + behind + this->downArrow;
			}
		}

		gitStr = this->bg("220") + this->sep + this->fg("0") + " " + this->alt + " " + gitStr + " " + this->fg("220");
	}

	return gitStr;
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

	return this->bg(col) + this->sep + this->fg("255") + " " + prompt + " " + this->fg(col);
}






