#include <err.h>
#include <string.h>

#include "config/config.h"

static int switch_action(struct config *conf, char *action)
{
    if (!conf)
        return 1;
    if (strcmp(action, "start") == 0)
    {
        // TODO
        return 0;
    }
    else if (strcmp(action, "stop") == 0)
    {
        // TODO
        return 0;
    }
    else if (strcmp(action, "reload") == 0)
    {
        // TODO
        return 0;
    }
    else if (strcmp(action, "restart") == 0)
    {
        // TODO
        //  Stop and start
        return 0;
    }
    else
        return 1;
}

static int parse_input(int argc, char *argv[], struct config *conf)
{
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
            errx(2, "http: invalid configuration file");
        if (strcmp("-a", argv[1]) == 0)
        {
            return switch_action(conf, argv[2]);
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
            errx(2, "http: invalid configuration file");
        }
        return 0;
        // Run server WITHOUT daemonizing it
    }
    // --dry-run and file
    if (argc == 3)
    {
        if (strcmp(argv[1], "--dry-run") == 0)
        {
            conf = parse_configuration(argv[2]);
            if (!conf)
            {
                errx(2, "http: invalid configuration file");
            }
            return 0;
        }
        return 1;
    }
    return parse_input(argc, argv, conf);
}
