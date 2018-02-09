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
#include <libconfig.h++>

const std::string Segments::angular = ""; // \uE0B0
const std::string Segments::angularThin = ""; // \uE0B1
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

Segments::Segments(std::string status, libconfig::Config& cfg)
{
	this->status = status;
	this->cfg = &cfg;
	
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
	return this->defaultColour(0) + this->angular + this->resetColour() + " ";
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

std::string Segments::getConf(std::string key, std::string defaultValue)
{
	const char* confValue;
	std::string outValue = "";

	try {
		if (this->cfg->exists(key)) {
			const libconfig::Setting &s = this->cfg->lookup(key);

			switch (s.getType()) {
				case libconfig::Setting::TypeString: {
					this->cfg->lookupValue(key, confValue);
					break;
				}
				case libconfig::Setting::TypeInt: {
					int intVal;
					this->cfg->lookupValue(key, intVal);
					confValue = std::to_string(intVal).c_str();
					break;
				}
				case libconfig::Setting::TypeFloat: {
					float floatVal = this->cfg->lookup(key);
					confValue = std::to_string(floatVal).c_str();
					break;
				}
				case libconfig::Setting::TypeBoolean: {
				bool boolVal = this->cfg->lookup(key);
					confValue = std::to_string(boolVal).c_str();
					break;
				}
				default: {
					std::cout << "Error in config file (" << key << ")";
					confValue = "";
					break;
				}
			}

			return confValue;
		}
	} catch (const libconfig::SettingNotFoundException &nfex) {
		std::cout << "Setting not found exception - " << key << std::endl;
	} catch (const libconfig::SettingTypeException &stex) {
		std::cout << "Setting type exception - " << key << std::endl;
	}

	return defaultValue;
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
	std::string fg = getConf("segments.timestamp.fg", "250");
	std::string bg = getConf("segments.timestamp.bg", "238");
	std::string sep = getConf("segments.timestamp.separator", "");

	char buf[12];
	auto time = std::chrono::system_clock::now();
	std::time_t hTime = std::chrono::system_clock::to_time_t(time);
	strftime(buf, sizeof(buf), " [%I:%M:%S]", localtime(&hTime));

	return this->bg(bg) + sep + this->fg(fg) + std::string(buf) + " ";
}

std::string Segments::username()
{
	std::string username = getenv("USER");

	std::string fg = getConf("segments.username.fg", "46");
	std::string bg = getConf("segments.username.bg", "240");
	std::string sep = getConf("segments.username.separator", "");

	if (this->isRoot()) {
		fg = getConf("segments.username.root.fg", "255");;
		bg = getConf("segments.username.root.bg", "204");
	}

	return this->bg(bg) + sep + this->fg(fg) + " \\u " + this->fg(bg);
}

std::string Segments::hostname()
{
	std::string fg = getConf("segments.hostname.fg", "46");
	std::string bg = getConf("segments.hostname.bg", "238");
	std::string sep = getConf("segments.hostname.separator", "");

	if (this->isRoot()) {
		fg = getConf("segments.hostname.root.fg", "255");
		bg = getConf("segments.hostname.root.bg", "204");
	}

	char *buffer;
	int bufSize = 100;
	buffer = (char*) malloc(bufSize);
	gethostname(buffer, bufSize);
	std::string hostname(buffer);
	free(buffer);

	return this->fg(fg) + sep + this->bg(bg) + " \\h " + this->fg(bg);
}

std::string Segments::cwd()
{
	// get cwd
	char *buffer; int bufSize = 1000;
	buffer = (char*) malloc(bufSize);
	getcwd(buffer, bufSize);
	std::string cwd(buffer);
	free(buffer);

	std::string sep = getConf("segments.cwd.separator", "");
	std::string fg = getConf("segments.cwd.fg", "255");
	std::string bg = getConf("segments.cwd.bg", "69");
	std::string shortenHomeDir = getConf("segments.cwd.shorten_home_dir", "1");
	std::string splitCwd = getConf("segments.cwd.split_cwd", "1");
	std::string splitCwdSeparator = getConf("segments.cwd.split_cwd_separator", this->angularThin);
	std::string splitCwdSeparatorColour = getConf("segments.cwd.split_cwd_separator_color", "240");

	// get colours if directory is not writeable
	if (access(cwd.c_str(), W_OK) != 0) {
		fg = getConf("segments.cwd.readonly.fg", "255");
		bg = getConf("segments.cwd.readonly.bg", "62");
	}

	// replace home dir path with ~
	if (shortenHomeDir == "1") {
		std::regex re("^" + this->getHomeDir());
		cwd = std::regex_replace(cwd, re, "~");
	}

	// replace slashes with chevrons
	if (splitCwd == "1") {
		bool prependRoot = cwd[0] == '/';

		std::regex slashes("\\/");
		cwd = std::regex_replace(cwd, slashes, this->fg(splitCwdSeparatorColour) + " " + splitCwdSeparator + " " + this->fg(fg));
		
		// add a slash to cwd if outside home dir
		if (prependRoot)
			cwd = "/" + cwd;
	}

	return this->bg(bg) + sep + this->fg(fg) + " " + cwd + " " + this->fg(bg);
}

std::string Segments::git()
{
	std::string sep = getConf("segments.git.separator", this->angular);
	std::string fg = getConf("segments.git.fg", "0");
	std::string bg = getConf("segments.git.bg", "220");
	std::string showLocal = getConf("segments.git.show_local", "1");
	std::string showRemote = getConf("segments.git.show_remote", "0");
	std::string showIcon = getConf("segments.git.show_icon", "1");
	std::string icon = getConf("segments.git.icon", this->alt);
	std::string iconColor = getConf("segments.git.icon_color", fg);
	std::string showPending = getConf("segments.git.show_pending", "1");
	std::string pendingColor = getConf("segments.git.pending_color", fg);
	std::string pendingPullIcon = getConf("segments.git.pending_pull_icon", this->downArrow);
	std::string pendingPushIcon = getConf("segments.git.pending_push_icon", this->upArrow);

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
			gitStr = "";

			if (showLocal == "1")
				gitStr += (std::string)local;

			if (executeCmd(&remote, remoteCmd, 100)) {
				if (showRemote == "1")
					gitStr += "[" + (std::string)remote + "]";

				// get commit counts
				std::string statusCmd = "git rev-list --left-right --count " + local + "..." + remote;

				if (showPending == "1") {
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
						gitStr += " " + ahead + pendingPushIcon;
					if (behind.compare("0"))
						gitStr += " " + behind + pendingPullIcon;
				}
			}
		}

		gitStr = this->bg(bg) + sep + this->fg(fg) + " " + icon + " " + gitStr + " " + this->fg(bg);
	}

	return gitStr;
}

std::string Segments::exit_status()
{
	return "" + this->status;
}

std::string Segments::prompt()
{
	std::string sep = getConf("segments.prompt.separator", this->angular);
	std::string userPrompt = getConf("segments.prompt.user_prompt", "$");
	std::string rootPrompt = getConf("segments.prompt.root_prompt", "#");

	std::string fg = getConf("segments.prompt.fg", "255");
	std::string bg = getConf("segments.prompt.bg", "240");

	if (this->status == "1") {
		fg = getConf("segments.prompt.error.fg", "0");
		bg = getConf("segments.prompt.error.bg", "204");
	}

	std::string prompt = (this->isRoot() ? rootPrompt : userPrompt);

	return this->bg(bg) + sep + this->fg(fg) + " " + prompt + " " + this->fg(bg);
}






