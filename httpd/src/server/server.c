#define _POSIX_C_SOURCE 200809L

#include "server.h"

#include <errno.h>
#include <stddef.h>
#include <time.h>

// Catch the request and respond
void communicate(int client_sock, struct config *conf)
{
    char buff[1024] = { 0 };
    ssize_t nread;
    struct string *input = string_create("", 0);
    struct request *req = NULL;
    while ((nread = recv(client_sock, buff, 1024, MSG_NOSIGNAL)) > 0)
    {
        string_concat_str(input, buff, nread);
        req = parse_request(input, conf);
        if (req != NULL)
        {
            if (req->error == 0)
            {
                req->error = 200; // OK
            }
            //log_write(conf, req, "Received");
            send_response(client_sock, conf, req);
            close(client_sock);
            return;
        }
        // send(client_sock, buff, nread, MSG_NOSIGNAL);
    }
    // req = parse_request(input, conf);
    // send_response(client_sock, conf, req, fd);
    close(client_sock);
    return;
}

int send_error(int client_sock, struct request *req)
{
    struct string *resp =string_create("HTTP/1.1 ", 9);
    if (req->error == 200)
    {
        string_concat_str(resp, " 200 OK\r\n", 9);
    }
    else if (req->error == 400)
    {
        string_concat_str(resp, " 400 Bad Request\r\n", 18);
    }
    else if (req->error == 405)
    {
        string_concat_str(resp, " 405 Method Not Allowed\r\n", 25);
    }
    else if (req->error == 505)
    {
        string_concat_str(resp, " 505 HTTP Version Not Supported\r\n", 33);
    }
    else if (req->error == 403)
    {
        string_concat_str(resp, " 403 Forbidden\r\n", 16);
    }
    else if (req->error == 404)
    {
        string_concat_str(resp, " 404 Not found\r\n", 16);
    }
    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    string_concat_str(resp, buf, strlen(buf));
    string_concat_str(resp, "\r\n", 2);
    send(client_sock, resp->data, resp->size, MSG_NOSIGNAL);
    return 0;
}

int send_correct(int client_sock, struct config *conf, struct request *req)
{
    if (client_sock && conf && req)
    {
        return 0;
    }
    return 1;
}

int send_response(int client_sock, struct config *conf, struct request *req)
{
    if (!conf)
        return -1;
    if (req->error != 200)
    {
        send_error(client_sock, req);
        return 1;
    }
    send_correct(client_sock, conf, req);
    return 0;
}

void link_accept(int sockfd, struct config *conf)
{
    if (listen(sockfd, SOMAXCONN) == -1)
    {
        return;
    }
    int client_sock;
    // Put signal handler TODO
    while (1)
    {
        client_sock = accept(sockfd, NULL, NULL);
        if (client_sock != -1)
        {
            create_logfile(conf);
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
            // perror("server: socket");
            continue;
        }
        if (setsockopt(listening_sock, SOL_SOCKET, SO_REUSEADDR, &oui,
                       sizeof(int))
            == -1)
        {
            // perror("server: setsockopt");
            exit(1);
        }
        // Fail to bind
        if (bind(listening_sock, tmp->ai_addr, tmp->ai_addrlen) == -1)
        {
            // perror("server: bind");
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
