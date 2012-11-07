#all: server.c client.c
#	gcc server.c -o server
#	gcc client.c -o client

all: server.o client.o fatunp.o
	gcc server.o fatunp.o -o server
	gcc client.o fatunp.o -o client
server.o: server.c
	gcc -c server.c 
client.o: client.c
	gcc -c client.c
fatunp.o: fatunp.c
	gcc -c fatunp.c

clean:
	rm client.o server.o fatunp.o server client 
pack:
	tar -cvf 9917259.tar client.c server.c fatunp.c fatunp.h makefile
