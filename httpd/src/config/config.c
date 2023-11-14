#define _POSIX_C_SOURCE 200809L

#include "config.h"

#include <ctype.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_value(char *line, int begin, int size)
{
    char *s = calloc(size, sizeof(char));
    int i = 0;
    while (line[begin + i] != '\n')
    {
        s[i] = line[begin + i];
        i++;
    }
    return s;
}

static int check_hosts(FILE *fp, struct config *conf)
{
    if (!conf->servers)
        return 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int name = 0;
    int ifport = 0;
    int ifip = 0;
    int ifroot = 0;
    while ((nread = getline(&line, &len, fp)) != -1) // End of file
    {
        if (fnmatch("server_name = *", line, 0) == 0) // struct string
        {
            char *s = copy_value(line, 14, nread);
            struct string *serv = string_create(s, nread - 15);
            conf->servers->server_name = serv;
            free(s);
            name = 1;
        }
        else if (fnmatch("port = *", line, 0) == 0)
        {
            conf->servers->port = copy_value(line, 7, nread);
            ifport = 1;
        }
        else if (fnmatch("ip = *", line, 0) == 0)
        {
            conf->servers->ip = copy_value(line, 5, nread);
            ifip = 1;
        }
        else if (fnmatch("root_dir = *", line, 0) == 0)
        {
            conf->servers->root_dir = copy_value(line, 11, nread);
            ifroot = 1;
        }
        else if (fnmatch("default_file = *", line, 0) == 0)
        {
            conf->servers->default_file = copy_value(line, 15, nread);
        }
    }
    free(line);
    return (name == 1 && ifport == 1 && ifip == 1 && ifroot == 1);
}

static int check_global(FILE *fp, struct config *conf)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int pidf = 0;
    while ((nread = getline(&line, &len, fp)) != -1) // EOF
    {
        if (fnmatch("log_file = *", line, 0) == 0)
        {
            conf->log_file = copy_value(line, 11, nread);
        }
        if (fnmatch("pid_file = *", line, 0) == 0)
        {
            conf->pid_file = copy_value(line, 11, nread);
            pidf = 1;
        }
        if (fnmatch("log = false\n", line, 0) == 0)
        {
            conf->log = false;
        }
        if (strcmp(line, "[[vhosts]]\n") == 0)
        {
            free(line);
            return (pidf == 1);
        }
    }
    free(line);
    return 0;
}

static FILE *open_file(const char *path)
{
    if (path == NULL)
        return NULL;
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return NULL;
    return fp;
}

// Result return function
static struct config *result(int error, int occ, struct config *conf, FILE *fp)
{
    if (error == 1)
    {
        config_destroy(conf);
        return NULL;
    }
    int close = fclose(fp);
    if (close == 0 && occ == 1)
    {
        conf->nb_servers = 1;
        return conf;
    }
    config_destroy(conf);
    return NULL;
}

// Main function
struct config *parse_configuration(const char *path)
{
    FILE *fp = open_file(path);
    if (fp == NULL)
        return NULL;
    struct config *conf = calloc(1, sizeof(struct config));
    if (!conf)
        return NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int occ = 0;
    int error = 0;
    while ((nread = getline(&line, &len, fp)) != -1) // End of file
    {
        if (occ == 0)
        {
            if (strcmp(line, "[global]\n") != 0)
            {
                error = 1;
                break;
            }
            conf->log = true;
            occ++;
            if (check_global(fp, conf) == 0)
            {
                error = 1;
                break;
            }
            else
            {
                conf->servers = calloc(1, sizeof(struct server_config));
                if (check_hosts(fp, conf) == 0)
                {
                    error = 1;
                    break;
                }
            }
        }
    }
    free(line);
    return result(error, occ, conf, fp);
}

/*
** @brief Free the config struct
** @param config The config struct to free
*/
void config_destroy(struct config *config)
{
    if (config)
    {
        free(config->pid_file);
        if (config->log_file)
        {
            free(config->log_file);
        }
        if (config->servers)
        {
            string_destroy(config->servers->server_name);
            if (config->servers->port)
            {
                free(config->servers->port);
            }
            if (config->servers->ip)
            {
                free(config->servers->ip);
            }
            if (config->servers->root_dir)
            {
                free(config->servers->root_dir);
            }
            if (config->servers->default_file)
            {
                free(config->servers->default_file);
            }
            free(config->servers);
        }
        free(config);
    }
}
