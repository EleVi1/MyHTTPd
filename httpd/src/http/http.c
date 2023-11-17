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
char *parse_line(struct string *str, size_t *begin)
{
    size_t len = 0;
    size_t i = *begin;
    char *res = calloc(str->size, sizeof(char));
    while (i < str->size)
    {
        if (str->data[i] == '\r')
        {
            if (i + 1 < str->size && str->data[i + 1] == '\n')
            {
                res = realloc(res, len + 1);
                *begin = i + 2;
                return res;
            }
            free(res);
            return NULL;
        }
        res[i] = str->data[i];
        len++;
        i++;
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
        req->error = 400; // No httpversion
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

// TO CONTINUE
struct request *parse_request(struct string *input, struct config *config)
{
    int line_nb = 0;
    char *line = NULL;
    size_t begin = 0;
    struct request *req = calloc(1, sizeof(struct request));
    while ((line = parse_line(input, &begin)) != NULL)
    {
        if (line_nb == 0) // Request line
        {
            req = check_line1(line, req);
            if (req->error != 0)
            {
                return req;
            }
            continue;
        }
    }
    // not a finished line
    return NULL;
}

// TODO
int send_response(struct request *request)
{
    if (request)
        return 0;
    return 1;
}

void free_request(struct request *request)
{
    if (request)
    {
        if (request->body)
        {
            free(request->body);
        }
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
