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

int main(int argc, char **argv)
{
    int ch, save_argc;
    char *cmd;
    char **args;
    struct config *cfg;

    cfg = parse_config();

    save_argc = argc;
    args = argv;
    
    argv += (argc - 3);
    argc = 3;
    ch = getopt(argc, argv, "f:");
    if (ch != -1) {
        char *filename;
        filename = get_option_copy(optarg);
        strncpy(cfg->data_location, filename, max_value_len);
        puts(filename);
        free(filename);
        save_argc -= 2;
    }
    argc = save_argc;
    argv = args;

    cmd = argv[1];

    if (strcmp(cmd, "add") == 0) {
        if (argc > 3)
            add_item_cmd(argv[2], argv[3], cfg);
        else
            add_item_cmd(argv[2], cfg->default_section, cfg);
    } else
    if (strcmp(cmd, "show") == 0) {
        if (argc > 2)
            show_section_cmd(argv[2], cfg);
        else
            show_all_sections_cmd(cfg);
    } else
    if (strcmp(cmd, "move") == 0) {
        move_item_cmd(argv[2], argv[3], argv[4], cfg);
    } else
    if (strcmp(cmd, "delete") == 0) {
        delete_item_cmd(argv[2], argv[3], cfg); 
    }


    return 0;
}
