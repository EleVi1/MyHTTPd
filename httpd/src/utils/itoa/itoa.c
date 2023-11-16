#include "itoa.h"

static int str_len(const char *s)
{
    int n = 0;
    while (s[n] != '\0')
    {
        n++;
    }
    return n;
}

static void str_revert(char *s)
{
    int n = str_len(s);
    int i = 0;
    char tmp;
    while (i < n / 2)
    {
        tmp = s[i];
        s[i] = s[n - 1 - i];
        s[n - 1 - i] = tmp;
        i++;
    }
}

char *my_itoa(int n, char *s, const char *base)
{
    if (n == 0)
    {
        s[0] = base[0];
        s[1] = '\0';
        return s;
    }
    int bis = n;
    if (n < 0)
        n = -n;
    int i = 0;
    int nb_base = str_len(base);
    int r;
    while (n > 0)
    {
        r = n % nb_base;
        s[i] = base[r];
        i++;
        n = n / nb_base;
    }
    if (bis < 0)
    {
        s[i] = '-';
        i++;
    }
    s[i] = '\0';
    str_revert(s);
    return s;
}
