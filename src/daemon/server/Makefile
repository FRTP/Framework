CC = g++
CFLAGS = -c -std=c++11 -lboost_system -lboost_thread -lpthread
LFLAGS = -lboost_system -lboost_thread -lpthread

all: frtpdaemon

frtpdaemon: main.o log.o parser.o daemon.o
	$(CC) $(LFLAGS) main.o log.o parser.o daemon.o -o frtpdaemon
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main.o
log.o: clog.hpp
	$(CC) $(CFLAGS) clog.hpp -o log.o
parser.o: cparser.hpp
	$(CC) $(CFLAGS) cparser.hpp -o parser.o
daemon.o: cdaemon.cpp cdaemon.h
	$(CC) $(CFLAGS) cdaemon.cpp -o daemon.o
clean:
	rm *.o frtpdaemon
