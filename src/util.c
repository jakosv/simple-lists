#include "util.h"

#include <string.h>
#include <stdlib.h>

void str_strip(char *str)
{
    char *s;
    int i, len;

    len = strlen(str);
    s = str;
    /* skip leading spaces */
    for (; *s == ' '; s++)
        len--;

    /* skip trailing spaces */
    for (i = len-1; i >= 0; i--)
        if (s[i] != ' ')
            break;
    len = i + 1;

    for (i = 0; i < len; i++)
        str[i] = s[i];
    str[len] = '\0';
}

char *str_concat(const char *s1, const char *s2)
{
    char *res;
    int s1_len, s2_len;
    s1_len = strlen(s1);
    s2_len = strlen(s2);
    res = malloc(s1_len + s2_len + 1);
    strcpy(res, s1);
    strcpy(res + s1_len, s2);
    res[s1_len + s2_len] = '\0';

    return res;
}
