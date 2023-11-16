#ifndef DAEMON_H
#define DAEMON_H

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int daemon_start(struct config *conf);
int daemon_stop(struct config *conf);
void daemon_reload(struct config *conf);
void daemon_restart(struct config *conf); // stop then start

#endif /* !DAEMON_H*/
