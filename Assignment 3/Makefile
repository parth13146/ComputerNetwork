CC=g++
CFLAGS=-c -Wall

all: hello

hello: servermain.o serverA.o serverB.o
	$(CC) servermain.o -o servermain
	$(CC) serverA.o -o serverA
	$(CC) serverB.o -o serverB

servermain.o: servermain.cpp
	$(CC) $(CFLAGS) servermain.cpp

serverA.o: serverA.cpp
	$(CC) $(CFLAGS) serverA.cpp

serverB.o: serverB.cpp
	$(CC) $(CFLAGS) serverB.cpp

clean: 
	rm -rf *o servermain
	rm -rf *o serverA
	rm -rf *o serverB
