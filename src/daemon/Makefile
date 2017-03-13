CC=g++
override CFLAGS+=-c -std=c++11 -Wall -Wextra -Werror

all: datatype utility

datatype: datatype.cpp 
	$(CC) $(CFLAGS) -fPIC -DBOOST_LOG_DYN_LINK datatype.cpp -o build/datatype.o

utility: utility.cpp
	$(CC) $(CFLAGS) -fPIC utility.cpp -o build/utility.o
