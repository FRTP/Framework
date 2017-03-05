#!/bin/sh

if [ $1 = "help" ]; then
	echo "Usage: sh build.sh <target> [options]"
	echo -e "\n"
	echo "Targets:"
	echo -e "all \t build all targets"
	echo -e "daemon \t build server daemon and client lib"
	echo -e "server \t build server part"
	echo -e "client \t build client lib"
	echo -e "\n"
	echo "Options:"
	echo -e "--enable-testing \t  build with tests"
	echo -e "--debug \t\t  build debug version"
fi

if [ -n "$2" ]; then
	if [ $2 = "--enable-testing" ]; then
		echo Testing is not implemented yet
		exit 127
	elif [ $2 = "--debug" ]; then
		echo Building debug version
		make $1 CFLAGS=-g
	else
		echo Unknown option, use 'sh build.sh help' for more details
	fi 
else
	if [ $1 != "help" ]; then
		make $1
	fi
fi
