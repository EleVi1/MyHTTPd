#ifndef DAEMON_H
#define DAEMON_H

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../config/config.h"
#include "../utils/string/string.h"

int daemon_start(struct config *conf);
int daemon_stop(struct config *conf);
int daemon_restart(struct config *conf);

#endif /* !DAEMON_H*/
