CC = g++
CFLAGS = -c -std=c++11 -lboost_system -lboost_thread -lpthread
LFLAGS = -lboost_system -lboost_thread -lpthread

all: server client

server: servermain.o daemon.o
	$(CC) $(LFLAGS) servermain.o daemon.o -o frtpdaemon
servermain.o: server/main.cpp
	$(CC) $(CFLAGS) server/main.cpp -o servermain.o
clog.o: server/clog.hpp
	$(CC) $(CFLAGS) server/clog.hpp -o clog.o
parser.o: server/cparser.hpp
	$(CC) $(CFLAGS) server/cparser.hpp -o parser.o
daemon.o: server/cdaemon.cpp server/cdaemon.h
	$(CC) $(CFLAGS) server/cdaemon.cpp -o daemon.o

client: clientmain.o client.o
	$(CC) $(LFLAGS) -shared clientmain.o client.o -o libfrtpsrv.so
clientmain.o: client/main.cpp
	$(CC) $(CFLAGS) -fpic client/main.cpp -o clientmain.o
client.o: client/cclient.cpp client/cclient.h
	$(CC) $(CFLAGS) -fpic client/cclient.cpp -o client.o

clean:
	rm *.o frtpdaemon libfrtpsrv.so
