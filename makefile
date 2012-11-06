#all: server.c client.c
#	gcc server.c -o server
#	gcc client.c -o client

all: server.o client.o function.o
	gcc server.o function.o -o server
	gcc client.o function.o -o client
server.o: server.c
	gcc -c server.c 
client.o: client.c
	gcc -c client.c
function.o: function.c
	gcc -c function.c
