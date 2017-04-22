#!/bin/bash

function usage { 
	echo "Usage: sh build.sh -t <target> [options]"
	echo -e "\n"
	echo "Targets:"
	echo -e "all \t\t build all targets"
	echo -e "daemon \t\t build server daemon and client lib"
	echo -e "server \t\t build server part"
	echo -e "client \t\t build client lib"
	echo -e "clean \t\t clean build directory"
	echo -e "\n"
	echo "Options:"
	echo -e "-s \t\t build with tests"
	echo -e "-d \t\t build debug version"
	echo -e "-j <number> \t determine the number of parallel jobs"
	echo -e "-h \t\t show this help"
	exit 1
}

TARGET=""
ADDITIONAL_CFLAGS=""
ADDITIONAL_LFLAGS=""
JOBS=2
DEFINES=""
PYTHON_PATH=""

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
						if [ $TARGET = "client" ] || [ $TARGET = "daemon" ] \
						|| [ $TARGET = "server" ] || [ $TARGET = "all" ]; then
							PYTHON_PATH="$(find /usr/include -name pyconfig.h | head -1)"
							PYTHON_PATH=${PYTHON_PATH: : -10}
							echo Using python directory $PYTHON_PATH
							ADDITIONAL_CFLAGS="-I$PYTHON_PATH"
							PYTHON_VERSION="$(echo $PYTHON_PATH | egrep -oh '[0-9]{1}\.[0-9]{1}(m){0,1}')"
							ADDITIONAL_LFLAGS="-lpython$PYTHON_VERSION"
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
			ADDITIONAL_CFLAGS="-DTESTING $ADDITIONAL_CFLAGS"
			ADDITIONAL_LFLAGS="-lboost_unit_test_framework $ADDITIONAL_LFLAGS"
			echo Building with tests
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

if [ -z "$TARGET" ]; then
	usage
	exit 127
fi

make -j$JOBS $TARGET CFLAGS="$ADDITIONAL_CFLAGS" LFLAGS="$ADDITIONAL_LFLAGS"
