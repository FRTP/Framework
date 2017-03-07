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
	echo -e "-j \t determine the number of parallel jobs"
	echo -e "-h \t show this help"
	exit 1
}

TARGET=""
ADDITIONAL_CFLAGS=""
JOBS=2

while getopts "t:j:sdh" opt;
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
						if [ $TARGET = "client" ] || [ $TARGET = "daemon" ] || [ $TARGET = "all" ]; then
							PYTHON_PATH="$(find /usr/include -name pyconfig.h -printf "%h\n" | head -1)/"
							echo Using python directory $PYTHON_PATH
							ADDITIONAL_CFLAGS="-I$PYTHON_PATH"
						fi
					else
						echo Unknown target, aborting
						exit 127
					fi
					;;
			esac
			;;
		j)
			JOBS=$OPTARG
			;;
		s)
			echo Testing is not implemented yet
			exit 127
			;;
		d)
			ADDITIONAL_CFLAGS="-g $ADDITIONAL_CFLAGS"
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


make -j$JOBS $TARGET CFLAGS="$ADDITIONAL_CFLAGS"
