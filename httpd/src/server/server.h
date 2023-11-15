#ifndef SERVER_H
#define SERVER_H

#include <err.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../config/config.h"

void communicate(int client_sock);
void link_accept(int sockfd);
int initialize(char *ipv4, char *port);
int main_server(struct config *conf);

#endif /* !SERVER_H*/
