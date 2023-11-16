#define _POSIX_C_SOURCE 200809L

#include "daemon.h"

static size_t str_len(const char *s)
{
    size_t i = 0;
    while (s[i] != '\0')
    {
        i++;
    }
    return i;
}

// Get new pid stored in pid_file
// If file doesn't exist, create it (1)
// Otherwise check if the process is alive (2)
// If it is stop the binary and return 1 (3)
// Else delete and do as the first line (4)
int daemon_start(struct config *conf)
{
    pid_t pid = -1;
    FILE *fp;
    char *pid_file = conf->pid_file;
    char *new_pid = NULL;
    if (pid_file == NULL) //(1)
    {
        fp = fopen("httpd.pid", "w");
        pid = fork();
        // check pid TODO
        new_pid = itoa(pid, new_pid, 10);
        write(fp, new_pid, str_len(new_pid));
        conf->pid_file = "httpd.pid";
        fclose(fp);
    }
    fp = fopen(conf->pid_file, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    if ((nread = getline(&line, &len, fp)) != -1)
    {
        pid = atoi(line);
        // Check if process is alive
        if (/*TODO*/) //(2)
            return 1; //(3)
    }
    fclose(fp);
    conf->pid_file = NULL;
    daemon_start(conf); // (4)
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
    conf->pid_file = NULL;
    return 0;
}

void daemon_reload(struct config *conf)
{
    // TODO
}

void daemon_restart(struct config *conf)
{
    daemon_stop(conf);
    daemon_start(conf);
    // TODO
}
