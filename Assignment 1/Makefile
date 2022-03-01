CC=g++
CFLAGS=-c -Wall 

all: hello

hello: crc_tx.o crc_rx.o crc_vs_checksum.o
	$(CC) crc_tx.o -o crc_tx
	$(CC) crc_rx.o -o crc_rx
	$(CC) crc_vs_checksum.o -o crc_vs_checksum
	./crc_tx
	./crc_rx
	./crc_vs_checksum
	
	

crc_tx.o: crc_tx.cpp
	 $(CC) $(CFLAGS) crc_tx.cpp

crc_rx.o: crc_rx.cpp
	  $(CC) $(CFLAGS) crc_rx.cpp

crc_vs_checksum.o: crc_vs_checksum.cpp
	  $(CC) $(CFLAGS) crc_vs_checksum.cpp	   

clean: 
	rm -rf *o crc_tx
	rm -rf *o crc_rx
	rm -rf *o crc_vs_checksum