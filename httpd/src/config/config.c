#include "config.h"

#include <stdio.h>
#include <string.h>

static int str_len(char *s)
{
    int i = 0;
    while (s[i] != '\0')
    {
        i++;
    }
    return i;
}

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
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int occ = 0;
    while ((nread = getline(&line, &len, fp)) != -1) // End of file
    {
        // TODO
    }
}

static int check_global(FILE *fp, struct config *conf)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int occ = 0;
    int pidf = 0;
    while ((nread = getline(&line, &len, fp)) != -1) // End of file
    {
        // TODO
    }
    return (pidf == 1);
}

struct config *parse_configuration(const char *path)
{
    if (path == NULL)
        return NULL;
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return NULL;
    struct config *conf = calloc(1, sizeof(struct config));
    if (!config)
        return NULL;
    init_config(conf);
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int occ = 0;
    while ((nread = getline(&line, &len, fp)) != -1) // End of file
    {
        if (occ == 0)
        {
            if (strcmp(line, "[global]\n") != 0)
            {
                return NULL;
            }
            occ++;
            if (check_global(fp, conf) == 0)
            {
                return NULL;
            }
        }
        else if (strcmp(line, "[[vhosts]]\n") == 0)
        {
            if (!check_hosts(fp, conf))
            {
                return NULL;
            }
        }
    }
    int close = fclose(fp);
    if (close == 0)
    {
        return conf;
    }
    return NULL;
}

/*
** @brief Free the config struct
**
** @param config The config struct to free
*/
void config_destroy(struct config *config)
{
    if (config)
    {
        free(config->pid_file);
        free(config->log_file);
        if (config->servers)
        {
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
