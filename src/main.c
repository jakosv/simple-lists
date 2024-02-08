#include "command.h"
#include "config.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

char *get_option_copy(const char *opt_str)
{
    char *str;
    int len;

    len = strlen(opt_str);
    str = malloc(len + 1);
    strcpy(str, opt_str);
    str[len] = '\0';

    return str;
}

int parse_filename_argument(int argc, char **argv, struct config *cfg)
{
    int ch;
    ch = getopt(argc, argv, "f:");
    if (ch != -1) {
        char *filename;
        filename = get_option_copy(optarg);
        strncpy(cfg->data_location, filename, max_value_len);
        free(filename);
    }
    return optind;
}

int main(int argc, char **argv)
{
    int skip_argc;
    struct config *cfg;

    cfg = parse_config();
    skip_argc = parse_filename_argument(argc, argv, cfg);
    argc -= skip_argc;
    argv += skip_argc;

    perform_command(argc, argv, cfg);

    return 0;
}
