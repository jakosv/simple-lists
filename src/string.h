#ifndef STRING_H_SENTRY
#define STRING_H_SENTRY

struct string;

struct string *string_init();
void string_free(struct string *str);
void string_clear(struct string *str);
int string_length(struct string *str);
void string_add_char(char ch, struct string *str);
void string_copy(struct string *dst, const struct string *src);
void string_to_c_string(struct string *str);
void string_strip(struct string *str);
const char *get_string_data(const struct string *str);

#endif
