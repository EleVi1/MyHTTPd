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
            fprintf(fd, "%s [%s] received %s from 218.20.4.2\n", buf,
                    server_name, msg);
        }
        else
        {
            printf("%s [%s] received %s from 218.20.4.2\n", buf, server_name,
                   msg);
        }
    }
    else if (req->type == HEAD)
    {
        if (fd)
        {
            fprintf(fd, "%s [%s] received HEAD on '%s' from  218.20.4.2\n", buf,
                    server_name, req->target->data);
        }
        else
        {
            printf("%s [%s] received HEAD on '%s' from 218.20.4.2\n", buf,
                   server_name, req->target->data);
        }
    }
    else
    {
        if (fd)
        {
            fprintf(fd, "%s [%s] received GET on '%s' from 218.20.4.2\n", buf,
                    server_name, req->target->data); // struct string
        }
        else
        {
            printf("%s [%s] received GET on '%s' from 218.20.4.2\n", buf,
                   server_name, req->target->data);
        }
    }
    if (fd)
    {
        fclose(fd);
    }
    free(server_name);
}

static void fill_end(FILE *fd, struct request *req)
{
    if (req->error == 400)
    {
        fprintf(fd, "\n");
    }
    else
    {
        fprintf(fd, " for UNKNOWN on '%s'\n", req->target->data);
    }
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
    if (req->error == 400 || req->error == 405)
    {
        fprintf(fd, "%s, [%s] responding with %d to 218.20.4.2", buf,
                server_name, req->error);
        fill_end(fd, req);
        fclose(fd);
        free(server_name);
        return;
    }
    if (req->type == HEAD)
    {
        if (fd)
        {
            fprintf(
                fd,
                "%s [%s] responding with %d to 218.20.4.2 for HEAD on '%s'\n",
                buf, server_name, req->error, req->target->data);
        }
        else
        {
            printf(
                "%s [%s] responding with %d to 218.20.4.2 for HEAD on '%s'\n",
                buf, server_name, req->error, req->target->data);
        }
    }
    else
    {
        if (fd)
        {
            fprintf(
                fd,
                "%s [%s] responding with %d to 218.20.4.2 for GET on '%s'\n",
                buf, server_name, req->error, req->target->data);
        }
        else
        {
            printf("%s [%s] responding with %d to 218.20.4.2 for GET on '%s'\n",
                   buf, server_name, req->error, req->target->data);
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
