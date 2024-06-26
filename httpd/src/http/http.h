#ifndef HTTP_H
#define HTTP_H

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
    struct string *target;
    size_t body_len;
};

struct request *parse_request(struct string *input, struct config *config);
void free_request(struct request *request);

#endif /* !HTTP_H*/
