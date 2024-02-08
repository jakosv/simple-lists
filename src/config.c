#include "config.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILENAME ".slistrc"
#define DATA_FILENAME "slist_data"

#define DATA_LOCATION   "data_location"
#define DEFAULT_SECTION "default_section"

#define DEFAULT_SECTION_VALUE "unsorted"

enum { key_value_delimiter = '=' };

enum { max_key_len = 256 };

static struct config *get_default_config()
{
    char *data_dir, *data_path;
    struct config *cfg;

    data_dir = getenv("HOME");
    if (!data_dir)
        data_dir = getenv("PWD");
    data_path = str_concat(data_dir, "/"DATA_FILENAME); 

    cfg = malloc(sizeof(*cfg));
    strncpy(cfg->data_location, data_path, max_value_len);
    cfg->data_location[max_value_len] = '\0';
    strncpy(cfg->default_section, DEFAULT_SECTION_VALUE, max_value_len);
    cfg->default_section[max_value_len] = '\0';

    free(data_path);
    return cfg;
}

static void save_config(const char *path, const struct config *cfg)
{
    FILE *file;

    file = fopen(path, "w");

    fprintf(file, DATA_LOCATION" = %s\n", cfg->data_location);
    fprintf(file, DEFAULT_SECTION" = %s\n", cfg->default_section);

    fclose(file);
}

static char *get_config_path()
{
    const char *config_dir;
    char *config_path;
    
    config_dir = getenv("HOME");
    if (!config_dir)
        config_dir = getenv("PWD");
    config_path = str_concat(config_dir, "/"CONFIG_FILENAME); 

    return config_path;
}

static int parse_quotes(char *str, int str_len, char quote_char, 
                        FILE *file)
{
    int is_backslash, ch;
    is_backslash = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == quote_char && !is_backslash)
            break;
        if (is_backslash)
            is_backslash = 0;
        if (ch == '\\') {
            is_backslash = 1;
            continue;
        }
        str[str_len] = ch; 
        str_len += 1;
        if (str_len == max_value_len)
            break;
    }
    str[str_len] = '\0';
    
    return str_len;
}

static void read_spaces(FILE *file)
{
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch != ' ' && ch != '\n') {
            ungetc(ch, file);
            break;
        }
    }
}

static int parse_key(char *key, FILE *file)
{
    int ch, key_len;
    read_spaces(file);
    key_len = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == key_value_delimiter)
            break;
        key[key_len] = ch;
        key_len += 1;
        if (key_len == max_key_len)
            break;
    }

    key[key_len] = '\0'; 
    return key_len;
}

static int parse_value(char *value, FILE *file)
{
    int ch, value_len;
    value_len = 0;
    read_spaces(file);
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            break;
        } else
        if (ch == '"' || ch == '\'') {
            int res = parse_quotes(value, value_len, ch, file);
            if (res != 0)
                return res;
            continue;
        }
        value[value_len] = ch; 
        value_len++;
        if (value_len == max_value_len)
            break;
    }
    value[value_len] = 0; 
    return value_len;
}

static void save_default_config(const char *path, const struct config *cfg)
{
    int ch;
    printf("Do you want to create default config %s? (yes/no): ", path);
    ch = getchar();
    if (ch == 'y')
        save_config(path, cfg);
}

static void parse_config_value_by_key(const char *key, FILE *file, 
                                      struct config *cfg)
{
    if (strcmp(key, DATA_LOCATION) == 0) {
        parse_value(cfg->data_location, file);
        str_strip(cfg->data_location);
    } else 
    if (strcmp(key, DEFAULT_SECTION) == 0) {
        parse_value(cfg->default_section, file);
        str_strip(cfg->default_section);
    }    
}

struct config *parse_config()
{
    FILE *file;
    char *config_path;
    struct config *cfg;
    char key[max_key_len+1];
    int res;
    
    cfg = get_default_config();
    config_path = get_config_path();

    file = fopen(config_path, "r");
    if (!file) {
        save_default_config(config_path, cfg);
        goto quit;
    }

    while ((res = parse_key(key, file)) > 0) {
        str_strip(key);
        parse_config_value_by_key(key, file, cfg);
    }
    fclose(file);

quit:
    free(config_path);
    return cfg;
}
