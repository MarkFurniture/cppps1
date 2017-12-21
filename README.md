# C++ PS1

C++ implemented dynamic PS1 prompt for bash, inspired by [powerline-shell](https://github.com/b-ryan/powerline-shell).

###### Why not powerline-shell?
1. Primarily, pedagogy.
2. I only have need of a subset of powerline-shell's functionality, and wanted to run a performant PS1 prompt without requiring a daemon in the background. Running powerline-shell without a daemon is subject to python's startup time and performs IO that is superfluous to my needs, which slows down the prompt generation and makes it possible to interrupt the prompt generation and spit out exception traces to the console, hence this simplified version. This should be impossible to interrupt with keyboard input while generating prompt.

## Installation:

1. Clone this repo:
```
$ git clone https://github.com/MarkFurniture/cppps1.git
```
2. Build binary:
Using script:
```
# using build script
$ cd cppps1
$ ./bld.sh

# OR

# manually
$ cd cppps1
$ mkdir build
$ g++ -Wall -g -Werror -std=c++11 ps1.cpp segments.cpp -o ./build/cppps1
```
2. Make sure executable is in $PATH:
```
$ ln -s $(pwd)/build/cppps1 /usr/local/bin/cppps1
```
3. Make sure binary is executable:
```
$ chmod +x ./build/cppps1
```
4. Add to ~/.bashrc or ~/.bash_profile:
```
function _update_ps1() {
    # add your preferred prefix to the prompt, if any
    CPPPS1_PREFIX=""
    PS1="$(cppps1 $? $CPPPS1_PREFIX)"
}

if [ "$TERM" != "linux" ]; then
    PROMPT_COMMAND="_update_ps1"
fi
```
5. Source ~/.bashrc or ~/.bash_profile or restart your shell:
```
$ . ~/.bashrc
```

## Adding segments
###### segments.h
1. Your function which will be called by PS1::generate(). You may add other functions but this one MUST have the following signature:
```
std::string yourSegment();
```
2. Any other function signatures of functions you will add to supplement your segment.

###### segments.cpp
1. Your main segment function:
```
std::string yourSegment();
```
2. Add pointer to your function in Segments::funcMap():
```
// Remember the pointer type is: std::string (Segments::*fnPtr)()
// The key will be what you add to the options object to enable/disable your segment.
this->fnMap["yourSegment"] = &Segments::yourSegment;
```
3. Any other functions you have declared in Segments.h.

###### ps1.cpp
1. Modify PS1::getOptions() to include the name of your segment. This should be the key which you used in Segments::fnMap.

## Tips
###### Adding colour
1. You can use Segments::fg() and Segments::bg() to insert colours while building your string. This is the preferred method because it doesn't add overhead and can be used while building each segment without losing performance.
```
// segments.cpp
std::string Segments::yourSegment()
{
	// white text (255) on blue background (31)
	std::string segmentText = this->fg("255") + this->bg("31") + "your text";
	return segmentText;
}
```
2. You can use a template string to add colours to your prompt build your prompt if you want to keep it easier to read while developing it. This will do a regex_replace of all generated colour tokens with actual terminal colour escape sequences. This will add some overhead but the string will be easier to read in its raw format. You can either call this directly in your segment function or add it to ps1.cpp to call it once for the completed string.
```
// segments.cpp
std::string Segments::yourSegment()
{
	// white text (255) on blue background (31)
	std::string segmentText = "{f255}{b31}your text";
	return this->replaceColours(segmentText);
}
```