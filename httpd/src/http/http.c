#define _POSIX_C_SOURCE 200809L

#include "http.h"

static size_t str_len(char *s)
{
    size_t i = 0;
    while (s[i] != '\0')
    {
        i++;
    }
    return i;
}

// Store the string before \r\n
// if not return NULL
char *parse_line(struct string *str, size_t *begin, size_t *len)
{
    size_t j = 0;
    size_t i = *begin;
    char *res = calloc(str->size, sizeof(char));
    if (str->data[i] == '\r' && i + 1 < str->size && str->data[i + 1] == '\n')
    {
        res = realloc(res, 3);
        res[j] = '\r';
        *len = 3;
        res[j + 1] = '\n';
        return res;
    }
    while (i < str->size)
    {
        if (str->data[i] == '\r')
        {
            if (i + 1 < str->size && str->data[i + 1] == '\n')
            {
                res = realloc(res, *(len) + 1);
                *begin = i + 2;
                return res;
            }
            free(res);
            return NULL;
        }
        res[j] = str->data[i];
        *(len) += 1;
        i++;
        j++;
    }
    free(res);
    return NULL;
}

static struct request *check_version(struct request *req, char *line)
{
    if (fnmatch("HTTP/*", line, 0) != 0)
    {
        req->error = 400;
    }
    else
    {
        req->error = 505;
    }
    return req;
}

static struct request *check_line1(char *line, struct request *req)
{
    int tmp = str_len(line);
    if (strlen(line) < 3)
    {
        req->error = 400; // Bad request
        return req;
    }
    if (memcmp(line, "GET", 3) == 0)
    {
        req->type = GET;
    }
    else if (memcmp(line, "HEAD", 4) == 0)
    {
        req->type = HEAD;
    }
    else
    {
        req->error = 405; // Method not allowed
        return req;
    }
    size_t offset = strcspn(line, " ");
    tmp = tmp - offset - 1;
    if (tmp < 0)
    {
        req->error = 400;
        return req;
    }
    line = line + offset + 1;
    size_t url_size = strcspn(line, " ");
    tmp = tmp - url_size - 1;
    if (tmp < 0)
    {
        req->error = 400;
        return req;
    }
    char *url = calloc(url_size + 1, sizeof(char));
    for (size_t i = 0; i < url_size; i++)
    {
        url[i] = line[i];
    }
    url[url_size] = '\0';
    req->target = url;
    line = line + url_size + 1;
    if (strcspn(line, " ") == 0)
    {
        req->error = 400;
        free(url);
        return req;
    }
    else if (strcmp("HTTP/1.1", line) == 0)
    {
        return req;
    }
    else
        return check_version(req, line);
}

// Check content_length
static struct request *fill_length(char *line, struct request *req)
{
    int i = 0;
    while (line[i] != '\0')
    {
        if (line[i] < '0' || line[i] > '9')
        {
            req->error = 400;
        }
        i++;
    }
    req->body_len = atoi(line);
    return req;
}

static size_t get_len(char *line, char delim)
{
    size_t i = 0;
    while (line[i] != delim)
    {
        i++;
    }
    return i;
}

static struct request *fill_name(struct request *req, struct string *c)
{
    char *res = calloc(c->size, sizeof(char));
    for (size_t i = 0; i < c->size; i++)
    {
        res[i] = c->data[i];
    }
    req->host_name = res;
    return req;
}

// Host: [ <server_name> | <IP address> | <IP>:<Port> ]
static struct request *check_host(char *line, struct request *req,
                                  struct config *conf, size_t *len)
{
    struct server_config *tmp = conf->servers;
    char *copy = line;
    size_t n = conf->nb_servers;
    size_t i = 0;
    while (i < n)
    {
        if (string_compare_n_str(tmp->server_name, line, *len) == 0)
        {
            req = fill_name(req, tmp->server_name);
            return req;
        }
        if (strcmp(line, tmp->ip) == 0)
        {
            req = fill_name(req, tmp->server_name);
            return req;
        }
        if (fnmatch("*:*", line, 0) == 0)
        {
            size_t len = get_len(line, ':');
            char *val = NULL;
            if (len == str_len(tmp->ip)) // check IP address
            {
                val = calloc(len, sizeof(char));
                val = memcpy(val, line, len);
                if (memcmp(tmp->ip, val, len) == 0)
                {
                    copy += (strcspn(line, ":") + 1);
                    len = get_len(copy, '\0');
                    if (len == str_len(tmp->port)) // check port value
                    {
                        val = memcpy(val, copy, len);
                        if (memcmp(tmp->port, val, len) == 0)
                        {
                            req = fill_name(req, tmp->server_name);
                            free(val);
                            return req;
                        }
                        copy = line;
                    }
                }
                free(val);
            }
            return req;
        }
        tmp++;
    }
    req->error = 400; // bad request right ?
    return req;
}

static struct request *check_headers(char *input, struct request *req,
                                     struct config *conf, size_t *len)
{
    char *line = input;
    if (fnmatch("Content-Length: *", line, 0) == 0)
    {
        if (*len > 16)
        {
            line += (strcspn(line, " ") + 1);
            req = fill_length(line, req);
        }
        else
        {
            req->error = 400;
        }
        return req;
    }
    if (fnmatch("Host: *", line, 0) == 0)
    {
        if (str_len(input) > 6)
        {
            line += (strcspn(line, " ") + 1);
            req = check_host(line, req, conf, len);
        }
        else
        {
            req->error = 400;
        }
        return req;
    }
    return req;
}

static size_t fetch_begin(struct string *input)
{
    size_t i = 0;
    while (i < input->size)
    {
        if (input->data[i] == '\r')
        {
            if (i + 3 < input->size && input->data[i + 1] == '\n')
            {
                if (input->data[i + 2] == '\r' && input->data[i + 3] == '\n')
                {
                    return i + 4;
                }
            }
        }
        i++;
    }
    return 0;
}

// Count if there are enough characters after /r/n/r/n
static int msg_is_ok(struct string *input, struct request *req)
{
    size_t begin = fetch_begin(input);
    if (input->size - begin < req->body_len)
    {
        return -1;
    }
    return 0;
}

// Find a way to detect a ligne with only /r/n
struct request *parse_request(struct string *input, struct config *config)
{
    if (!config)
        return NULL;
    int line_nb = 0;
    char *line = "";
    size_t begin = 0;
    size_t len = 0;
    struct request *req = calloc(1, sizeof(struct request));
    while ((line = parse_line(input, &begin, &len)) != NULL
           && strcmp("\r\n", line) != 0)
    {
        if (line_nb == 0) // Request line
        {
            req = check_line1(line, req);
            line_nb++;
        }
        else // Field line
        {
            req = check_headers(line, req, config, &len);
        }
        if (req->error != 0)
        {
            free(line);
            return req;
        }
        free(line);
        len = 0;
        continue;
    }
    if (strcmp(line, "\r\n") == 0)
    {
        if (req->host_name != NULL && req->target != NULL)
        {
            free(line);
            if (req->body_len == 0)
            {
                return req;
            }
            if (msg_is_ok(input, req) != 0)
            {
                free(req);
                return NULL;
            }
            return req;
        }
        else
        {
            req->error = 400; // Bad request
            free(line);
            return req;
        }
    }
    // not a finished line or CRLF headers ending
    free(req);
    free(line);
    return NULL;
}

void free_request(struct request *request)
{
    if (request)
    {
        if (request->target)
        {
            free(request->target);
        }
        if (request->host_name)
        {
            free(request->host_name);
        }
    }
}
