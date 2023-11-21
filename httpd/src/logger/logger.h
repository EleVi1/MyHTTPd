#ifndef LOGGER_H
#define LOGGER_H

#include "../config/config.h"
#include "../http/http.h"
#include "../utils/string/string.h"

struct logger
{
    FILE *fd;
};

void create_logfile(struct config *conf);
void log_write(struct config *conf, struct request *req, char *msg);

#endif /* !LOGGER_H */
