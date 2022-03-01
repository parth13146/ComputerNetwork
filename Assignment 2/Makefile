CC=g++
CFLAGS=-c -Wall

all: hello

hello: servermain.o client.o
	$(CC) servermain.o -o servermain
	$(CC) client.o -o client

servermain.o: servermain.cpp
	$(CC) $(CFLAGS) servermain.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp

clean: 
	rm -rf *o servermain
	rm -rf *o client
