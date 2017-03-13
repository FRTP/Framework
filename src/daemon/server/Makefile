CC=g++
override CFLAGS+=-c -std=c++11 -Wall -Wextra -Werror
override LFLAGS+=-lboost_log -lpthread -lboost_system -lboost_thread -lcrypto

server: servermain.o cdaemon.o
	$(CC) ../build/datatype.o ../build/utility.o build/servermain.o build/cdaemon.o -o build/frtpdaemon $(LFLAGS)
servermain.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o build/servermain.o
cdaemon.o: cdaemon.cpp
	$(CC) $(CFLAGS) -DBOOST_LOG_DYN_LINK cdaemon.cpp -o build/cdaemon.o
