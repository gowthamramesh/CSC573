CC=gcc
LIBS=-lpthread

all: server q5

q5: client.o

server: archive-udp-server.o

client.o: 
	$(CC) -o client.o client.c
 
archive-udp-server.o:
	$(CC) -o archive-udp-server.o archive-udp-server.c

archive-tcp-server.o:
	$(CC) -o archive-tcp-server.o archive-tcp-server.c -lpthread
clean: 
	rm -rf *.o
