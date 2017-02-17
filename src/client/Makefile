CC = g++
CFLAGS = -c -std=c++11 -fpic -lboost_system -lboost_thread 
LFLAGS = -shared -lboost_system -lboost_thread -lpthread 

all: clientlib

clientlib: main.o client.o
	$(CC) $(LFLAGS) main.o client.o -o libfrtpsrv.so
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main.o
client.o: cclient.cpp cclient.h
	$(CC) $(CFLAGS) cclient.cpp -o client.o
clean:
	rm *.o libfrtpsrv.so
