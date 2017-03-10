CC=g++
override CFLAGS+=-c -std=c++11 -Wall -Wextra -Werror
override LFLAGS+=-lpthread -lboost_system -lboost_thread
SERVERLFLAGS+=-lcrypto
CLIENTLFLAGS+=-lboost_python

all: daemon

daemon: server client

server: servermain.o cdaemon.o datatype.o
	$(CC) servermain.o cdaemon.o datatype.o -o frtpdaemon $(LFLAGS) $(SERVERLFLAGS)
servermain.o: daemon/server/main.cpp
	$(CC) $(CFLAGS) daemon/server/main.cpp -o servermain.o
cdaemon.o: daemon/server/cdaemon.cpp daemon/server/cdaemon.h
	$(CC) $(CFLAGS) daemon/server/cdaemon.cpp -o cdaemon.o
datatype.o: daemon/datatype.cpp daemon/datatype.h
	$(CC) $(CFLAGS) daemon/datatype.cpp -o datatype.o

client: clientmain.o client.o ccommand.o datatype.o
	$(CC) -shared clientmain.o client.o ccommand.o datatype.o -o libfrtpsrv.so $(LFLAGS) $(CLIENTLFLAGS)
clientmain.o: daemon/client/main.cpp
	$(CC) $(CFLAGS) -fPIC daemon/client/main.cpp -o clientmain.o
client.o: daemon/client/cclient.cpp daemon/client/cclient.h
	$(CC) $(CFLAGS) -fPIC daemon/client/cclient.cpp -o client.o
ccommand.o: daemon/client/ccommand.cpp daemon/client/ccommand.h
	$(CC) $(CFLAGS) -fPIC daemon/client/ccommand.cpp -o ccommand.o
datatype.o: daemon/datatype.cpp daemon/datatype.h
	$(CC) $(CFLAGS) daemon/datatype.cpp -o datatype.o

clean:
	rm *.o frtpdaemon libfrtpsrv.so
