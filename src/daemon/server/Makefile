CC=g++
override CFLAGS+=-c -std=c++11 -Wall -Wextra -Werror
override LFLAGS+=-lboost_log_setup -lboost_log -lpthread -lboost_system -lboost_python\
		 -lboost_thread -lboost_regex -lcrypto -lsqlite3

server: servermain.o cdaemon.o ccommand.o
	$(CC) ../build/context.o ../build/datatype.o ../build/utility.o build/ccommand.o build/servermain.o build/cdaemon.o\
		-o build/frtpdaemon $(LFLAGS)
servermain.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o build/servermain.o
cdaemon.o: cdaemon.cpp
	$(CC) $(CFLAGS) -DBOOST_LOG_DYN_LINK cdaemon.cpp -o build/cdaemon.o
ccommand.o: ccommand.cpp
	$(CC) $(CFLAGS) ccommand.cpp -o build/ccommand.o
