#!/usr/bin/env bash

SRC_DIR=$(cd "$(dirname "$0")"; pwd)

if [ "$SRC_DIR" != "$(pwd)" ]; then
	echo "Could not detect source files. Please make sure you are in the project root."
	exit 1
fi

if [ ! -d bin ]; then
	mkdir bin
fi

g++ -Wall -g -Werror -std=c++11 -o bin/cppps1 -I include src/*.cpp
