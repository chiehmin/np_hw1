#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>   // struct sockaddr, struct sockaddr_in...
#include <arpa/inet.h>    // inet_ntoa, htons, htonl...
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 1024

int parse(const char* src, char* dst);
ssize_t readline(int fd, void *vptr, size_t maxlen);
