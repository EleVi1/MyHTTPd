#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <time.h>

void create_logfile(struct config *conf)
{
    if (!conf || !conf->log_file || !conf->log)
    {
        return;
    }
    if (conf->log_file)
    {
        FILE *file = fopen(conf->log_file, "a");
        if (file != NULL)
        {
            return;
        }
        fclose(file);
    }
}

static char *fill_error(int err)
{
    if (err == 400)
    {
        return "Bad request";
    }
    else if (err == 405)
    {
        return "Method Not Allowed";
    }
    else if (err == 505)
    {
        return "HTTP Version Not Supported";
    }
    else
        return "";
}

// IP address to add
static void write_recv(struct request *req, struct config *conf)
{
    FILE *fd = NULL;
    if (conf->log_file)
    {
        fd = fopen(conf->log_file, "a");
    }
    char buf[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    char *server_name =
        calloc(conf->servers->server_name->size + 1, sizeof(char));
    memcpy(server_name, conf->servers->server_name->data,
           conf->servers->server_name->size);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    if (req->error != 200)
    {
        char *msg = fill_error(req->error);
        if (fd)
        {
            fprintf(fd, "%s [%s] received %s from <Client IP>\n", buf,
                    server_name, msg);
        }
        else
        {
            printf("%s [%s] received %s from <Client IP>\n", buf, server_name,
                   msg);
        }
    }
    else if (req->type == HEAD)
    {
        if (fd)
        {
            fprintf(fd, "%s [%s] received HEAD on '%s' from <Client IP>\n", buf,
                    server_name, req->target);
        }
        else
        {
            printf("%s [%s] received HEAD on '%s' from <Client IP>\n", buf,
                   server_name, req->target);
        }
    }
    else
    {
        if (fd)
        {
            fprintf(fd, "%s [%s] received GET on '%s' from <Client IP>\n", buf,
                    server_name, req->target);
        }
        else
        {
            printf("%s [%s] received GET on '%s' from <Client IP>\n", buf,
                   server_name, req->target);
        }
    }
    if (fd)
    {
        fclose(fd);
    }
    free(server_name);
}

static void write_send(struct request *req, struct config *conf)
{
    FILE *fd = NULL;
    if (conf->log_file)
    {
        fd = fopen(conf->log_file, "a");
    }
    char buf[1000];
    char *server_name =
        calloc(conf->servers->server_name->size + 1, sizeof(char));
    memcpy(server_name, conf->servers->server_name->data,
           conf->servers->server_name->size);
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    if (req->type == HEAD)
    {
        if (fd)
        {
            fprintf(fd, "%s [%s] responding with %d to <Client IP> for HEAD\n",
                    buf, server_name, req->error);
        }
        else
        {
            printf("%s [%s] responding with %d to <Client IP> for HEAD\n", buf,
                   server_name, req->error);
        }
    }
    else
    {
        if (fd)
        {
            fprintf(fd, "%s [%s] responding with %d to <client ip> for get\n",
                    buf, server_name, req->error);
        }
        else
        {
            printf("%s [%s] responding with %d to <Client IP> for GET\n", buf,
                   server_name, req->error);
        }
    }
    if (fd)
    {
        fclose(fd);
    }
    free(server_name);
}

void log_write(struct config *conf, struct request *req, char *msg)
{
    if (!conf->log)
        return;
    if (strcmp(msg, "Received") == 0)
    {
        write_recv(req, conf);
    }
    else if (strcmp(msg, "Sent") == 0)
    {
        write_send(req, conf);
    }
}
