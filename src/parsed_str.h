#ifndef PARSED_STR_H_SENTRY
#define PARSED_STR_H_SENTRY

struct parsed_str;

struct parsed_str *parsed_str_init();
void parsed_str_free(struct parsed_str *str);
void parsed_str_clear(struct parsed_str *str);
int parsed_str_length(struct parsed_str *str);
void parsed_str_add_char(char ch, struct parsed_str *str);
void parsed_str_to_c_string(struct parsed_str *str);
void parsed_str_strip(struct parsed_str *str);
const char *get_parsed_str_data(const struct parsed_str *str);

#endif
