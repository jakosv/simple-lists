#ifndef CONFIG_H_SENTRY
#define CONFIG_H_SENTRY

enum { max_value_len = 256 };

struct config {
    char data_location[max_value_len+1];
    char default_section[max_value_len+1];
    char section_decoration[max_value_len+1];
    char item_decoration[max_value_len+1];
};

struct config *parse_config();

#endif
