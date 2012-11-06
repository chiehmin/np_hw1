#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

short str2short(char *ptr)
{
    int i;
    for(i = 0; i < strlen(ptr); i++)
        if(isdigit(ptr[i]) == 0)
            return -1;
    return atoi(ptr);
}

int main(int argc, char **argv)
{
    int i, j, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set rset, allset;
    char buf[1024];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    short serv_port;


    if(argc == 2 && (serv_port = str2short(argv[1])) >= 0)
    {
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(serv_port);

        bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

        listen(listenfd, 1024);

        maxfd = listenfd;
        maxi = -1;
        memset(client, -1, sizeof(client));
        FD_ZERO(&allset);
        FD_SET(listenfd, &allset);

        while(1)
        {
            rset = allset;
            nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
            if(FD_ISSET(listenfd, &rset))
            {
                clilen = sizeof(cliaddr);
                connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);

                printf("connect from %s, port %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
                printf("%d\n", connfd);
                for(i = 0; i < FD_SETSIZE; i++)
                    if(client[i] < 0) 
                    {
                        client[i] = connfd;
                        break;
                    }
                FD_SET(connfd, &allset);
                if(connfd > maxfd)
                    maxfd = connfd;
                if(i > maxi)
                    maxi = i;
                if(--nready <= 0)
                    continue;
            }
            for(i = 0; i <= maxi; i++)
            {
                if((sockfd = client[i]) < 0)
                    continue;
                if(FD_ISSET(sockfd, &rset))
                {
                    if((n = read(sockfd, buf, 1024)) == 0)
                    {
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i] = -1;
                        printf("connection of %d closed\n", sockfd);
                    }
                    else
                    {
                        for(j = 0; j <= maxi; j++)
                        {
                            if(client[j] >= 0)
                                write(client[j], buf, n);
                        }
                    }

                    if(--nready <= 0)
                        break;
                }
            }
        }

    }
    else
        puts("usage: server port_number");
    return 0;
}
