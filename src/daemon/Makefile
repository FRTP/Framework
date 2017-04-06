CC=g++
override CFLAGS+=-c -std=c++11 -Wall -Wextra -Werror

all: utility context

utility: utility.cpp
	$(CC) $(CFLAGS) -fPIC utility.cpp -o build/utility.o

context: ccontext.cpp
	$(CC) $(CFLAGS) -fPIC ccontext.cpp -o build/context.o
