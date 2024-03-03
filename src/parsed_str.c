#include "parsed_str.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

struct parsed_str {
    char *data;
    int length, capacity;
};

struct parsed_str *parsed_str_init()
{
    struct parsed_str *str;

    str = malloc(sizeof(struct parsed_str));
    str->length = 0;
    str->capacity = 2;
    str->data = malloc(str->capacity);

    return str;
}

void parsed_str_free(struct parsed_str *str)
{
    free(str->data);
    str->length = 0;
    str->capacity = 0;
    free(str);
}

void parsed_str_clear(struct parsed_str *str)
{
    str->length = 0;
}

int parsed_str_length(struct parsed_str *str)
{
    return str->length;
}

void parsed_str_add_char(char ch, struct parsed_str *str)
{
    if (str->length >= str->capacity-1) {
        str->capacity *= 2;
        str->data = realloc(str->data, str->capacity);
    }
    str->data[str->length] = ch;
    str->length += 1;
}

void parsed_str_to_c_string(struct parsed_str *str)
{
    str->data[str->length] = '\0';
}

void parsed_str_strip(struct parsed_str *str)
{
    parsed_str_to_c_string(str);
    str_strip(str->data);

    str->length = strlen(str->data);
}

const char *get_parsed_str_data(const struct parsed_str *str)
{
    return str->data;
}
