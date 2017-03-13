CC=g++
override CFLAGS+=-c -std=c++11 -Wall -Wextra -Werror
override LFLAGS+=-lpthread -lboost_system -lboost_thread -lcrypto -lboost_python

client: clientmain.o client.o ccommand.o
	$(CC) -shared build/clientmain.o build/client.o build/ccommand.o ../build/datatype.o -o build/libfrtpsrv.so $(LFLAGS)
clientmain.o: main.cpp
	$(CC) $(CFLAGS) -fPIC main.cpp -o build/clientmain.o
client.o: cclient.cpp
	$(CC) $(CFLAGS) -fPIC cclient.cpp -o build/client.o
ccommand.o: ccommand.cpp
	$(CC) $(CFLAGS) -fPIC ccommand.cpp -o build/ccommand.o
