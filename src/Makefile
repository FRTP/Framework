CC = g++
CFLAGS = -c -std=c++11 -Wall -Wextra
LFLAGS = -lboost_system -lboost_thread -lpthread

all: server client

server: servermain.o daemon.o
	$(CC) servermain.o daemon.o -o frtpdaemon $(LFLAGS)
servermain.o: daemon/server/main.cpp
	$(CC) $(CFLAGS) daemon/server/main.cpp -o servermain.o
clog.o: daemon/server/clog.hpp
	$(CC) $(CFLAGS) daemon/server/clog.hpp -o clog.o
parser.o: daemon/server/cparser.hpp
	$(CC) $(CFLAGS) daemon/server/cparser.hpp -o parser.o
daemon.o: daemon/server/cdaemon.cpp daemon/server/cdaemon.h
	$(CC) $(CFLAGS) daemon/server/cdaemon.cpp -o daemon.o

client: clientmain.o client.o
	$(CC) -shared clientmain.o client.o -o libfrtpsrv.so $(LFLAGS)
clientmain.o: daemon/client/main.cpp
	$(CC) $(CFLAGS) -fpic daemon/client/main.cpp -o clientmain.o
client.o: daemon/client/cclient.cpp daemon/client/cclient.h
	$(CC) $(CFLAGS) -fpic daemon/client/cclient.cpp -o client.o

clean:
	rm *.o frtpdaemon libfrtpsrv.so
