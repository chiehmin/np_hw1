#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#define CMD_NUM 6

int parse(const char *src, char* dst)
{
    /*
     * return value:
     * 0 => /serv
     * 1 => /msg
     * 2 => /nick
     */
    char cmd[CMD_NUM][1024] = {"/serv", "/msg", "/nick", "/connect", "/who", "/quit"};
    int i, j;
    for(i = 0; i < CMD_NUM; i++)
    {
        for(j = 0; j < strlen(src) && j < strlen(cmd[i]); j++)
        {
            if(src[j] != cmd[i][j])
                break;
        }
        if(j == strlen(cmd[i]))
        {
            strcpy(dst, src + j + 1);
            return i;
        }
    }
    return -1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for(n = 1; n < maxlen; n++)
    {
        again:
        if((rc = read(fd, &c, 1)) == 1)
        {
            *ptr++ = c;
            if(c == '\n' || c == '\0')
                break;
        }
        else if(rc == 0)
        {
            *ptr = 0;
            return n - 1;
        }
        else
        {
            if(errno == EINTR)  
                goto again;
            return -1;
        }
    }
    *ptr = 0;
    return n;
}
