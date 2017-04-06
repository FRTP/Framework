CC=g++
override CFLAGS+=-c -std=c++11 -Wall -Wextra -Werror
override LFLAGS+=-lpthread -lboost_filesystem -lboost_system -lboost_thread -lcrypto -lboost_python

client: clientmain.o client.o ccommand.o datatype.o
	$(CC) -shared ../build/utility.o ../build/context.o build/datatype.o build/clientmain.o build/client.o\
		build/ccommand.o -o build/libfrtpsrv.so $(LFLAGS)
clientmain.o: main.cpp
	$(CC) $(CFLAGS) -fPIC main.cpp -o build/clientmain.o
client.o: cclient.cpp
	$(CC) $(CFLAGS) -fPIC cclient.cpp -o build/client.o
ccommand.o: ccommand.cpp
	$(CC) $(CFLAGS) -fPIC ccommand.cpp -o build/ccommand.o
datatype.o: datatype.cpp
	$(CC) $(CFLAGS) -fPIC datatype.cpp -o build/datatype.o
