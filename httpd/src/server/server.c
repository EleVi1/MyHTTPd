#define _POSIX_C_SOURCE 200809L

#include "server.h"

#include <errno.h>
#include <stddef.h>

// Do the echo action - Basic server
void communicate(int client_sock, struct config *conf)
{
    char buff[1024] = { 0 };
    ssize_t nread;
    struct string *input = string_create("", 0);
    struct request *req;
    size_t toread = 1024;
    while ((nread = recv(client_sock, buff, toread, MSG_NOSIGNAL)) > 0)
    {
        string_concat_str(req, buff, nread);
        if (toread == 1024)
        {
            req = parse_request(input, conf);
            if (req != NULL)
            {
                if (req->error != 0)
                {
                    send_response(req, conf);
                    return;
                }
                if (new->body_len > 0)
                {
                    toread = new->body_len;
                }
            }
        }
        else
        {
            toread -= nread;
        }
        if (toread == 0)
        {
            break;
        }
        // send(client_sock, buff, nread, MSG_NOSIGNAL);
    }
    req = parse_request(input, conf);
    send_response(req, conf);
    return;
}

void link_accept(int sockfd, struct config *conf)
{
    // Signal graceful shutdown
    /*
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    */
    if (listen(sockfd, SOMAXCONN) == -1)
    {
        return;
    }
    int client_sock;
    while (1)
    {
        client_sock = accept(sockfd, NULL, NULL);
        if (client_sock != -1)
        {
            communicate(client_sock, conf);
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
    link_accept(sockfd, conf);
    close(sockfd);
    config_destroy(conf);
    return 0;
}
