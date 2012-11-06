#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int max(int a, int b)
{
    if(a > b)
        return a;
    return b;
}

void str_cli(FILE *fp, int sockfd)
{
    int maxfdp1;
    fd_set rset;
    char sendline[1024], recvline[1024];
    int n;

    FD_ZERO(&rset);
    while(1)
    {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = max(fileno(fp),sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if(FD_ISSET(sockfd, &rset))
        {
            if(read(sockfd, recvline, 1024) == 0)
            {
                puts("error");
                exit(EXIT_FAILURE);
            }
            fputs(recvline, stdout);
        }
        if(FD_ISSET(fileno(fp), &rset))
        {
            if(fgets(sendline, 1024, fp) == NULL)
                return;
            write(sockfd, sendline, strlen(sendline) + 1);
        }
    }
}

int main(int argc, char** argv)
{
    int sockfd;
    struct sockaddr_in servaddr, localaddr;
    int len;

    if(argc != 3)
    {
        puts("usage: tcpcli <IPaddress> <serv_port>");
        exit(EXIT_FAILURE);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    
    len = sizeof(localaddr);
    getsockname(sockfd, (struct sockaddr*) &localaddr, &len);

    printf("Local IP address %s, port %d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
    str_cli(stdin, sockfd);
    
    close(sockfd);
    exit(0);
}
