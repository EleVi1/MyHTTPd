#define _POSIX_C_SOURCE 200809L

#include "server.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <time.h>

#include "../utils/itoa/itoa.h"

int loop = 1;

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
                req->error = 200;
            }
            send_response(client_sock, conf, req);
            close(client_sock);
            free_request(req); // Added
            string_destroy(input); // Added
            return;
        }
        // send(client_sock, buff, nread, MSG_NOSIGNAL);
    }
    // req = parse_request(input, conf);
    // send_response(client_sock, conf, req, fd);
    close(client_sock);
    string_destroy(input); // Added
    return;
}

static int send_error(int client_sock, struct request *req, struct string *str)
{
    string_destroy(str);
    struct string *resp = string_create("HTTP/1.1", 8);
    if (req->error == 400)
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
        string_concat_str(resp, " 404 Not Found\r\n", 16);
    }
    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    string_concat_str(resp, "Date: ", 6);
    string_concat_str(resp, buf, strlen(buf));
    string_concat_str(resp, "\r\n", 2);
    string_concat_str(resp, "Connection: close\r\n\r\n", 21);
    // string_concat_str(resp, "\0", 1);
    // printf("%s\n", resp->data);
    send(client_sock, resp->data, resp->size, MSG_NOSIGNAL);
    string_destroy(resp);
    return 0;
}

static size_t str_len(char *s)
{
    size_t i = 0;
    while (s[i] != '\0')
    {
        i++;
    }
    return i;
}

static int is_slash(char *s, size_t n)
{
    return (s[n - 1] == '/');
}

static int get_filesize(FILE *fd, char *body)
{
    char c;
    int s = 0;
    while ((c = fgetc(fd)) != EOF)
    {
        body[s] = c;
        s++;
    }
    return s;
}

static int send_correct(int client_sock, struct config *conf,
                        struct request *req)
{
    struct stat sb;
    size_t len = str_len(conf->servers->root_dir);
    struct string *name;
    if (is_slash(conf->servers->root_dir, len))
        name = string_create(conf->servers->root_dir, len - 1);
    else
        name = string_create(conf->servers->root_dir, len);
    string_concat_str(name, req->target->data, req->target->size);
    int stated = stat(name->data, &sb);
    if (stated == -1)
    {
        req->error = 404;
        return send_error(client_sock, req, name);
    }
    FILE *fd = fopen(name->data, "r");
    if (fd == NULL)
    {
        req->error = 403;
        return send_error(client_sock, req, name);
    }
    char *body = calloc(10000, sizeof(char));
    int size = get_filesize(fd, body);
    fclose(fd);

    struct string *resp = string_create("HTTP/1.1 200 OK\r\nDate: ", 23);

    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    string_concat_str(resp, buf, strlen(buf));

    if (size > 0)
    {
        string_concat_str(resp, "\r\nContent-length: ", 18);
        char *nb = calloc(10, sizeof(char));
        nb = my_itoa(size, nb, "0123456789");
        string_concat_str(resp, nb, str_len(nb));
        free(nb);
    }
    string_concat_str(resp, "\r\nConnection: close\r\n\r\n", 23);
    // string_concat_str(resp, "\0", 1);
    // printf("%s\n", resp->data);
    send(client_sock, resp->data, resp->size, MSG_NOSIGNAL);
    if (size > 0)
    {
        send(client_sock, body, size, MSG_NOSIGNAL);
    }
    free(body);
    string_destroy(resp);
    string_destroy(name); // Added
    return 0;
}

int send_response(int client_sock, struct config *conf, struct request *req)
{
    if (!conf)
        return -1;
    if (req->error != 200)
    {
        send_error(client_sock, req, NULL);
        return 1;
    }
    send_correct(client_sock, conf, req);
    return 0;
}

static void handler(int signal)
{
    switch (signal)
    {
    case SIGINT: {
        loop = 0;
    }
    default:
        break;
    }
}

void link_accept(int sockfd, struct config *conf)
{
    if (listen(sockfd, SOMAXCONN) == -1)
    {
        return;
    }
    int client_sock;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    // Initialize mask
    if (sigemptyset(&sa.sa_mask) < 0)
    {}
    if (sigaction(SIGINT, &sa, NULL) < 0)
    {}
    while (loop)
    {
        client_sock = accept(sockfd, NULL, NULL);
        if (client_sock != -1)
        {
            create_logfile(conf);
            communicate(client_sock, conf);
            close(client_sock);
        }
    }
    // printf("UWU\n");
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
            continue;
        }
        if (setsockopt(listening_sock, SOL_SOCKET, SO_REUSEADDR, &oui,
                       sizeof(int))
            == -1)
        {
            exit(1);
        }
        // Fail to bind
        if (bind(listening_sock, tmp->ai_addr, tmp->ai_addrlen) == -1)
        {
            close(listening_sock);
            continue;
        }
        break; // Succeed to bind
    }
    freeaddrinfo(server_info);
    if (!tmp)
    {
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
