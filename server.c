#include "fatunp.h"

struct user
{
    int fd;
    char nick[30], ip[30];
    int nick_set;
}client[FD_SETSIZE];


const char ask_nick[] = "/serv What’s your name?";

short str2short(char *ptr)
{
    int i;
    for(i = 0; i < strlen(ptr); i++)
        if(isdigit(ptr[i]) == 0)
            return -1;
    return atoi(ptr);
}

void bc_msg(int sockfd, const char* nick, const char* msg)
{
    char target[1024];

    sprintf(target, "/msg %s SAID: %s", nick, msg);

    write(sockfd, target, strlen(target) + 1);

}

void welcome_msg(int sockfd, const char* nick)
{
    struct sockaddr_in localaddr;
    int s_len;
    char target[1024];

    memset(&localaddr, 0, sizeof(localaddr));
    s_len = sizeof(localaddr);

    getsockname(sockfd, (struct sockaddr*) &localaddr, &s_len);

    sprintf(target, "/serv Hello %s, welcome! ServerIP:%s:%d", nick, inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));

    write(sockfd, target, strlen(target) + 1);
}

void set_nick(char *ptr, int s, int maxi)
{
    int i, tmp;
    const char err_nick1[] = "/serv This name has been used by others.";
    const char err_nick2[] = "/serv Username can only consists of 2~12 digits or English letters."; 
    char target[1024], oldnick[1024];

    tmp = 0;
    if(strlen(ptr) < 2 || strlen(ptr) > 12)
        tmp = 2;
    for(i = 0; i < strlen(ptr); i++)
        if(isalnum(ptr[i]) == 0)
        {
            tmp = 2;
            break;
        }

    for(i = 0; i <= maxi; i++)
        if(i != s && client[i].fd >= 0 && strcmp(ptr, client[i].nick) == 0)
        {
            tmp = 1;
            break;
        }
    if(tmp == 1)
        write(client[s].fd, err_nick1, strlen(err_nick1) + 1);
    else if(tmp == 2)
        write(client[s].fd, err_nick2, strlen(err_nick2) + 1);
    else
    {
        strcpy(oldnick, client[s].nick);
        strcpy(client[s].nick, ptr);
        if(client[s].nick_set == 0)
        {
            welcome_msg(client[s].fd, client[s].nick);
            client[s].nick_set = 1;
            for(i = 0; i <= maxi; i++)
                if(i != s && client[i].fd >= 0 && client[i].nick_set == 1)
                {
                    sprintf(target, "/serv %s is online.", client[s].nick);
                    write(client[i].fd, target, strlen(target) + 1);
                }
        }
        else
        {
            sprintf(target, "/serv You're now known as %s.", client[s].nick);
            write(client[s].fd, target, strlen(target) + 1);
            for(i = 0; i <= maxi; i++)
                if(i != s && client[i].fd >= 0 && client[i].nick_set == 1)
                {
                    sprintf(target, "/serv %s is now known as %s.", oldnick, client[s].nick);
                    write(client[i].fd, target, strlen(target) + 1);
                }
        }
    }
    return;
}

void my_close(int s, int maxi, fd_set *allset)
{       
    char target[1024];
    int i;

    strcpy(target, "/quit");
    write(client[s].fd, target, strlen(target) + 1);

    if(client[s].nick_set == 1)
    {
        sprintf(target, "/serv %s is offline.", client[s].nick);
        for(i = 0; i <= maxi; i++)
            if(i != s && client[i].fd >= 0)
                write(client[i].fd, target, strlen(target) + 1);
    }

    close(client[s].fd);
    FD_CLR(client[s].fd, allset);
    printf("Connection of %d closed\n", client[s].fd);
    client[s].fd = -1;
    client[s].nick_set = 0;
    memset(client[s].nick, 0, sizeof(client[s].nick));
    memset(client[s].ip, 0, sizeof(client[s].ip));
    return;
}

int main(int argc, char **argv)
{
    int i, j, maxi, maxfd, listenfd, connfd, sockfd;
    int nready;
    ssize_t n;
    fd_set rset, allset;
    char buf[1024], buf2[1024];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    short serv_port;
    int cmd, tmp;


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

                printf("Connect from %s, port %d, sockfd %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), connfd);

                for(i = 0; i < FD_SETSIZE; i++)
                    if(client[i].fd < 0) 
                    {
                        client[i].fd = connfd;
                        memset(client[i].nick, 0, sizeof(client[i].nick));
                        client[i].nick_set = 0;
                        sprintf(client[i].ip, "%s:%d", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
                        break;
                    }

                FD_SET(connfd, &allset);

                write(connfd, ask_nick, strlen(ask_nick) + 1);

                if(connfd > maxfd)
                    maxfd = connfd;
                if(i > maxi)
                    maxi = i;
                if(--nready <= 0)
                    continue;
            }
            for(i = 0; i <= maxi; i++)
            {
                if((sockfd = client[i].fd) < 0)
                    continue;
                if(FD_ISSET(sockfd, &rset))
                {
                    if(read(sockfd, buf, 1024) == 0)
                        my_close(i, maxi, &allset);
                    else
                    {
                        buf[strlen(buf) - 1] = 0;
                        if(client[i].nick_set == 0)
                            set_nick(buf, i, maxi);
                        else if((cmd = parse(buf, buf2)) == 2)
                            set_nick(buf2, i, maxi);
                        else if(cmd == 3)   //connect
                        {
                        }
                        else if(cmd == 4)   //who
                        {
                            for(j = 0; j <= maxi; j++)
                                if(client[j].fd >= 0 && client[j].nick_set == 1)
                                {
                                    sprintf(buf2, "/serv %s %s", client[j].nick, client[j].ip);
                                    write(client[i].fd, buf2, strlen(buf2) + 1);
                                }
                        }
                        else if(cmd == 5)   //quit
                            my_close(i, maxi, &allset);
                        else
                        {
                            for(j = 0; j <= maxi; j++)
                                if(client[j].fd >= 0 && client[j].nick_set == 1)
                                    bc_msg(client[j].fd, client[i].nick, buf);
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
