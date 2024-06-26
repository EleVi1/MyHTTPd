#include <err.h>
#include <string.h>

#include "config/config.h"
#include "daemon/daemon.h"
#include "logger/logger.h"
#include "server/server.h"

static int switch_action(struct config *conf, char *action)
{
    int res = 0;
    if (!conf)
        return 1;
    if (strcmp(action, "start") == 0)
    {
        res = daemon_start(conf);
        if (res == 0)
        {
            return main_server(conf);
        }
        if (res == -1)
        {
            return 1;
        }
        return 0;
    }
    else if (strcmp(action, "stop") == 0)
    {
        res = daemon_stop(conf);
        return res;
    }
    else if (strcmp(action, "reload") == 0)
    {
        return 0;
    }
    else if (strcmp(action, "restart") == 0)
    {
        daemon_stop(conf);
        res = daemon_start(conf);
        if (res == 0)
        {
            return main_server(conf);
        }
        if (res == -1)
        {
            return 1;
        }
        return 0;
        /*res = daemon_restart(conf);
        if (res == -1)
        {
            return -1;
        }
        else if (res == 0)
        {
            return main_server(conf);
        }
        return 0;*/
    }
    return 1;
}

static int parse_input(int argc, char *argv[], struct config *conf)
{
    int res = 0;
    if (argc == 5) // Full
    {
        if (strcmp(argv[1], "--dry-run") == 0)
        {
            conf = parse_configuration(argv[4]);
            if (conf != NULL)
            {
                return 0;
            }
            else
            {
                errx(2, "httpd: invalid configuration file");
            }
        }
        else
        {
            errx(1, "httpd: invalid command");
        }
    }
    if (argc == 4)
    {
        conf = parse_configuration(argv[3]);
        if (!conf)
        {
            return 2;
        }
        // errx(2, "http: invalid configuration file");
        if (strcmp("-a", argv[1]) == 0)
        {
            res = switch_action(conf, argv[2]);
            // config_destroy(conf);
            return res;
        }
        errx(1, "httpd: invalid command");
    }
    errx(1, "httpd: invalid command");
}

// Parsing to do for the command line
int main(int argc, char *argv[])
{
    struct config *conf = NULL;
    if (argc > 5 || argc < 2) // Invalid
        errx(1, "httpd: invalid command");
    // Just configuration file
    if (argc == 2)
    {
        conf = parse_configuration(argv[1]);
        if (!conf)
        {
            config_destroy(conf);
            return 2;
        }
        // Run server without deamonizing it
        return main_server(conf);
    }
    // --dry-run and file
    if (argc == 3)
    {
        if (strcmp(argv[1], "--dry-run") == 0)
        {
            conf = parse_configuration(argv[2]);
            if (!conf)
            {
                config_destroy(conf);
                return 2;
            }
            // printf("Valid configuration file\n");
            config_destroy(conf);
            return 0;
        }
        return 1;
    }
    int res = parse_input(argc, argv, conf);
    config_destroy(conf);
    return res;
}
