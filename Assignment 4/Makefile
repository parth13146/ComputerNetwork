CC=g++
CFLAGS=-c -Wall

all: hello

hello: servermain.o serverA.o serverB.o client.o
	$(CC) servermain.o -o servermain
	$(CC) serverA.o -o serverA
	$(CC) serverB.o -o serverB
	$(CC) client.o -o client

servermain.o: servermain.cpp
	$(CC) $(CFLAGS) servermain.cpp

serverA.o: serverA.cpp
	$(CC) $(CFLAGS) serverA.cpp

serverB.o: serverB.cpp
	$(CC) $(CFLAGS) serverB.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp

clean: 
	rm -rf *o servermain
	rm -rf *o serverA
	rm -rf *o serverB
	rm -rf *o client

