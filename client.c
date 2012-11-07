#include "fatunp.h"

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
    char sendline[1024], recvline[1024], msg[1024];
    int n, cmd;

    FD_ZERO(&rset);
    while(1)
    {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = max(fileno(fp),sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if(FD_ISSET(sockfd, &rset))
        {
            if(readline(sockfd, recvline, 1024) == 0)
            {
                puts("error");
                exit(EXIT_FAILURE);
            }
            if((cmd = parse(recvline, msg)) == 0)
            {
                printf("[Server] %s\n", msg);
            }
            else if(cmd == 1)
            {
                puts(msg);
            }
            else if(cmd == 5)
            {
                exit(EXIT_SUCCESS);
            }
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
    struct sockaddr_in servaddr;
    int len, cmd;
    char ipaddr[1024], port[1024], buf[1024], buf2[1024];

    if(argc != 3)
    {
        puts("It is not connected to any server");
        puts("Please use /connect <IP address> <Port number>");
        while(fgets(buf, 1024, stdin))
        {
            cmd = parse(buf, buf2);
            if(cmd == 3)
            {
                sscanf(buf2, "%s %s", ipaddr, port);
                break;
            }
            puts("It is not connected to any server");
            puts("Please use /connect <IP address> <Port number>");
        }

    }
    else
    {
        strcpy(ipaddr, argv[1]);
        strcpy(port, argv[2]);
    }
    

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, ipaddr, &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    
    str_cli(stdin, sockfd);
    
    close(sockfd);
    exit(0);
}
