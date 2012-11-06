all: server.c client.c
	gcc server.c -o server
	gcc client.c -o client

#all: server.o client.o io.o
#	gcc server.o io.o -o server
#	gcc client.o io.o -o client
#server.o: server.c
#	gcc -c server.c 
#client.o: client.c
#	gcc -c client.c
#io.o: io.c
#	gcc -c io.c
