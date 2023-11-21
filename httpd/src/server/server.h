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
#include "../http/http.h"
#include "../logger/logger.h"

void communicate(int client_sock, struct config *conf);
int send_response(int client_sock, struct config *conf, struct request *req);
void link_accept(int sockfd, struct config *conf);
int initialize(char *ipv4, char *port);
int main_server(struct config *conf);

#endif /* !SERVER_H*/
