#ifndef HTTP_H
#define HTTP_H

#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../config/config.h"
#include "../utils/string/string.h"

enum method
{
    GET,
    HEAD,
    INVALID,
};

struct request
{
    enum method type;
    int error;
    char *host_name;
    char *target;
    size_t body_len;
    char *body;
};

struct request *parse_request(struct string *input, struct config *config);
int send_request(struct request *req, struct config *conf);
void free_request(struct request *request);

#endif /* !HTTP_H*/
