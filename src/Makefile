CC = g++
CFLAGS = -c -std=c++11 -Wall -Wextra -Werror
LFLAGS = -lboost_system -lboost_thread -lpthread

all: daemon

daemon: server client

server: servermain.o cdaemon.o
	$(CC) servermain.o cdaemon.o -o frtpdaemon $(LFLAGS)
servermain.o: daemon/server/main.cpp
	$(CC) $(CFLAGS) daemon/server/main.cpp -o servermain.o
cdaemon.o: daemon/server/cdaemon.cpp daemon/server/cdaemon.h
	$(CC) $(CFLAGS) daemon/server/cdaemon.cpp -o cdaemon.o

client: clientmain.o client.o
	$(CC) -shared clientmain.o client.o -o libfrtpsrv.so $(LFLAGS)
clientmain.o: daemon/client/main.cpp
	$(CC) $(CFLAGS) -fpic daemon/client/main.cpp -o clientmain.o
client.o: daemon/client/cclient.cpp daemon/client/cclient.h
	$(CC) $(CFLAGS) -fpic daemon/client/cclient.cpp -o client.o

clean:
	rm *.o frtpdaemon libfrtpsrv.so
