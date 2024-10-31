#include "string.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

struct string {
    char *data;
    int length, capacity;
};

struct string *string_init()
{
    struct string *str;

    str = malloc(sizeof(struct string));
    str->length = 0;
    str->capacity = 2;
    str->data = malloc(str->capacity);

    return str;
}

void string_free(struct string *str)
{
    free(str->data);
    str->length = 0;
    str->capacity = 0;
    free(str);
}

void string_clear(struct string *str)
{
    str->length = 0;
}

int string_length(struct string *str)
{
    return str->length;
}

void string_copy(struct string *dst, const struct string *src)
{
    if (dst->capacity < src->capacity) {
        dst->capacity = src->capacity;
        free(dst->data);
        dst->data = malloc(dst->capacity);
    }
    strncpy(dst->data, src->data, src->length);
    dst->length = src->length;
}

void string_add_char(char ch, struct string *str)
{
    if (str->length >= str->capacity-1) {
        str->capacity *= 2;
        str->data = realloc(str->data, str->capacity);
    }
    str->data[str->length] = ch;
    str->length += 1;
}

void string_to_c_string(struct string *str)
{
    str->data[str->length] = '\0';
}

void string_strip(struct string *str)
{
    string_to_c_string(str);
    str_strip(str->data);

    str->length = strlen(str->data);
}

const char *get_string_data(const struct string *str)
{
    return str->data;
}
