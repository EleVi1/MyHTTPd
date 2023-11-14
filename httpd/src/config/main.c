#include "string.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    FILE *fp = fopen("../test1", "r");
    if (fp == NULL)
        return 1;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int i = 0;
    size_t rank = 0;
    int written = 0;
    while ((nread = getline(&line, &len, fp)) != -1) // End of line
    {
        printf("%s", line);
        printf("Read: %d\n", nread);
    }
    int close = fclose(fp);
    if (close == 0)
        return 0;
    return 1;
}
