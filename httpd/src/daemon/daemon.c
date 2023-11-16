#include "daemon.h"

int daemon_start(struct config *conf)
{
    int pid = fork();
}

// Check if the file exists and if the process is ALIVE
// if it is => SIGINT sent to the server
// if not or after that => delete pid_file return 0
int daemon_stop(struct config *conf)
{
    if (conf->pid_file != NULL)
    {
        if (/* Process is alive */)
        {
            // Send SIGINT
        }
    }
    free(conf->pid_file);
    conf->pid_file = NULL;
    return 0;
}

void daemon_reload(struct config *conf)
{
    // TODO
}

void daemon_restart(struct config *conf)
{
    // Stop then start
    // TODO
}
