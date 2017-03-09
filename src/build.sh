#!/bin/bash

function usage { 
	echo "Usage: sh build.sh -t <target> [options]"
	echo -e "\n"
	echo "Targets:"
	echo -e "all \t build all targets"
	echo -e "daemon \t build server daemon and client lib"
	echo -e "server \t build server part"
	echo -e "client \t build client lib"
	echo -e "clean \t clean build directory"
	echo -e "\n"
	echo "Options:"
	echo -e "-s \t build with tests"
	echo -e "-d \t build debug version"
	echo -e "-h \t show this help"
	exit 1
}

TARGET=""
ADDITIONAL_CFLAGS=""

while getopts "t:sdh" opt;
do
	case $opt in
		t)
			case $OPTARG in
				"") 	echo Target was not specified, aborting
					exit 127
					;;
				*)	if [ $OPTARG = "daemon" ] || [ $OPTARG = "server" ] \
					|| [ $OPTARG = "client" ] || [ $OPTARG = "clean" ] \
					|| [ $OPTARG = "all" ]; then
						TARGET="$OPTARG"
					else
						echo Unknown target, aborting
						exit 127
					fi
					;;
			esac
			;;
		s)
			echo Testing is not implemented yet
			exit 127
			;;
		d)
			ADDITIONAL_CFLAGS+=-g
			echo Building debug version
			;;
		h) 
			usage
			;;
		*)
			usage
			;;
	esac
done

make $TARGET CFLAGS=$ADDITIONAL_CFLAGS
