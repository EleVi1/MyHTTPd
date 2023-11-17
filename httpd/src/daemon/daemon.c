#define _POSIX_C_SOURCE 200809L

#include "daemon.h"

#include "../utils/itoa/itoa.h"

/*static char *parse(char *line, ssize_t len)
{
    char *res = calloc(len, sizeof(char));
    for (int i = 0; i < len; i++)
    {
        res[i] = line[i];
    }
    return res;
}*/

int daemon_start(struct config *conf)
{
    pid_t pid = -1;
    FILE *fp;
    char *new_pid = calloc(10, sizeof(char));
    fp = fopen(conf->pid_file, "r");
    if (fp == NULL) // No existent file
    {
        pid = fork();
        if (pid == -1 || pid == 0)
        {
            return (pid == -1) ? 1 : 0;
        }
        // In the parent: store the pid
        fp = fopen(conf->pid_file, "w+");
        new_pid = my_itoa(pid, new_pid, "0123456789");
        fwrite(new_pid, 1, strlen(new_pid), fp);
        free(new_pid);
        fclose(fp);
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    if ((nread = getline(&line, &len, fp)) != -1) // Not empty
    {
        // fprintf(stderr, "PID: %s\n", line);
        pid = atoi(line);
        if (kill(pid, 0) != -1) // Process alive
        {
            free(line);
            fclose(fp);
            return 1;
        }
    }
    free(line);
    fclose(fp);
    pid = fork();
    if (pid == -1)
    {
        return 1;
    }
    if (pid == 0) // in child
    {
        return 0; // Launch server
    }
    // In the parent, store the pid
    fp = fopen(conf->pid_file, "w+");
    new_pid = my_itoa(pid, new_pid, "0123456789");
    fwrite(new_pid, 1, strlen(new_pid), fp);
    free(new_pid);
    fclose(fp);
    return 1;
}

// Check if the file exists and if the process is ALIVE
// if it is => SIGINT sent to the server
// if not or after that => delete pid_file return 0 (remove)
int daemon_stop(struct config *conf)
{
    FILE *fp;
    pid_t pid;
    fp = fopen(conf->pid_file, "r");
    if (fp == NULL) // error
        return 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    if ((nread = getline(&line, &len, fp)) != -1)
    {
        pid = atoi(line);
        if (kill(pid, 0) != -1) // Process alive
        {
            kill(pid, SIGINT); // Kill process
        }
        free(line);
        fclose(fp);
        remove(conf->pid_file);
        return 0;
    }
    free(line);
    fclose(fp);
    remove(conf->pid_file);
    return 0;
}

int daemon_restart(struct config *conf)
{
    daemon_stop(conf);
    return daemon_start(conf);
}
