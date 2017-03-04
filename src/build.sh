#!/bin/sh

if [ -n "$2" ]; then
	if [ $2 = "--enable-testing" ]; then
		echo Testing is not implemented yet
		exit 127
	fi
fi

make $1
