#define _POSIX_C_SOURCE 200809L
#define BACKLOG 128

#include "server.h"

#include <errno.h>
#include <stddef.h>

// Do the echo action - Basic server
void communicate(int client_sock)
{
    char buff[1024] = { 0 };
    ssize_t nread;
    while ((nread = recv(client_sock, buff, 1024, MSG_NOSIGNAL)) > 0)
    {
        send(client_sock, buff, nread, MSG_NOSIGNAL);
    }
    return;
}

void link_accept(int sockfd)
{
    if (listen(sockfd, BACKLOG) == -1)
    {
        return;
    }
    int client_sock;
    while (1)
    {
        client_sock = accept(sockfd, NULL, NULL);
        if (client_sock != -1)
        {
            communicate(client_sock);
            close(client_sock);
        }
    }
}

int initialize(char *ipv4, char *port)
{
    int listening_sock;
    struct addrinfo hints;
    struct addrinfo *server_info;
    struct addrinfo *tmp;
    int oui = 1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int res = getaddrinfo(ipv4, port, &hints, &server_info);
    if (res != 0)
    {
        return -1;
    }
    // create socket and bind to the first possible
    for (tmp = server_info; tmp; tmp = tmp->ai_next)
    {
        listening_sock = socket(tmp->ai_family, tmp->ai_socktype, 0);
        // Fail to socket
        if (listening_sock == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(listening_sock, SOL_SOCKET, SO_REUSEADDR, &oui,
                       sizeof(int))
            == -1)
        {
            perror("server: setsockopt");
            exit(1);
        }
        // Fail to bind
        if (bind(listening_sock, tmp->ai_addr, tmp->ai_addrlen) == -1)
        {
            perror("server: bind");
            close(listening_sock);
            continue;
        }
        break; // Succeed to bind
    }
    freeaddrinfo(server_info);
    if (!tmp)
    {
        // fprintf(stderr, "server: failed to bind\n");
        return -1;
    }
    return listening_sock;
}

int main_server(struct config *conf)
{
    int sockfd = initialize(conf->servers->ip, conf->servers->port);
    if (sockfd == -1)
    {
        config_destroy(conf);
        return -1;
    }
    link_accept(sockfd);
    close(sockfd);
    config_destroy(conf);
    return 0;
}
