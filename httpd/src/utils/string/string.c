#include "string.h"

#include <stdlib.h>
#include <string.h>

/*
 ** @brief Create new string struct from char * and size
 **        Be careful, the argument str will not be deallocated and thus you
 **        can call this function with a static char *
 ** @return the newly allocated string
 */
struct string *string_create(const char *str, size_t size)
{
    struct string *new = calloc(1, sizeof(struct string));
    if (!new)
        return NULL;
    char *s = calloc(size, sizeof(char));
    memcpy(s, str, size);
    new->size = size;
    new->data = s;
    return new;
}

/*
 ** @brief Act like memcmp(3) but for struct string and char *
 ** @return
 */
int string_compare_n_str(const struct string *str1, const char *str2, size_t n)
{
    if (n == 0)
        return 0;
    if (str1->size > n)
        return 1;
    else if (n > str1->size)
        return -1;
    if (n == 0)
        return 0;
    size_t i = 0;
    while (i < n)
    {
        if (str1->data[i] != str2[i])
        {
            return str1->data[i] - str2[i];
        }
        i++;
    }
    return 0;
}

/*
 ** @brief Concat a char * with its size in a struct string
 */
void string_concat_str(struct string *str, const char *to_concat, size_t size)
{
    if (size == 0)
        return;
    if (!str)
    {
        str = string_create(to_concat, size);
    }
    else
    {
        size_t bef = str->size;
        str->data = realloc(str->data, str->size + size);
        if (!str->data)
            return;
        for (size_t i = 0; i < size; i++)
        {
            str->data[bef + i] = to_concat[i];
        }
        str->size = bef + size;
    }
}

void string_destroy(struct string *str)
{
    if (str)
    {
        free(str->data);
        free(str);
    }
}
