#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        errx(1, "Bad usage ./epoll <pipe_name>");
    }
    int file = epoll_create1(0);
    if (file == -1)
    {
        errx(1, "Fail to create epoll");
    }
    struct epoll_event *evmts = calloc(1, sizeof(struct epoll_event));
    evmts->events = EPOLLET | EPOLLIN;
    int fd = open(argv[1], O_RDONLY);
    int ctl = epoll_ctl(file, EPOLL_CTL_ADD, fd, evmts); // 0 for stdin
    if (ctl == -1)
    {
        close(fd);
        free(evmts);
        return -1;
    }
    int res;
    char *str = calloc(80, sizeof(char));
    while (1)
    {
        res = epoll_wait(file, evmts, 1, -1);
        if (EPOLLIN & evmts->events)
        {
            if (res == -1 || res == 0)
            {
                break;
            }
            read(fd, str, 80);
            if (strcmp(str, "ping") == 0)
            {
                printf("pong!\n");
            }
            else if (strcmp(str, "pong") == 0)
            {
                printf("ping!\n");
            }
            else if (strcmp(str, "quit") == 0)
            {
                printf("quit\n");
                res = 0;
                break;
            }
            else if (strcmp(str, "") != 0)
            {
                printf("Unknown: %s\n", str);
            }
            memset(str, 0, 80);
        }
    }
    epoll_ctl(file, EPOLL_CTL_DEL, fd, evmts);
    free(evmts);
    free(str);
    close(fd);
    return (res == -1) ? 1 : 0;
}
