#include "command.h"
#include "config.h"
#include "item_dbl.h"
#include "section_dbl.h"
#include "file_data.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define HELP "`slist [-f file] <command> <item> <section>`\n"\
    "`slist` show all sections\n"\
    "`slist [section_pos|section_name]` show section\n"\
    "`slist add [item]` add item to default section\n"\
    "`slist add [item] [section_pos|section_name]` add item to section\n"\
    "`slist del [item_pos] [section_pos|section_name]` delete item\n"\
    "`slist mv [item_pos] [section_pos|section_name] [target_section]` "\
    "move item to another section\n"\
    "`slist cp [item_pos] [section_pos|section_name] [target_section]` "\
    "copy item\n"
    

#define ADD_CMD "add"
#define MOVE_CMD "mv"
#define COPY_CMD "cp"
#define DELETE_CMD "del"
#define HELP_CMD "help"

static void command_error(const char *err_fmt, ...)
{
    va_list vl;
    va_start(vl, err_fmt);
    vfprintf(stderr, err_fmt, vl);
    va_end(vl);

    exit(1);
}

static int str_to_pos(const char *str_pos)
{
    int pos;
    pos = atoi(str_pos);
    return pos;
}

static void show_section(const struct section_dbl_node *section)
{
    struct item_dbl_node *item;
    int pos;

    printf("==== %s ====\n", section->section_name);
    pos = 1;
    for (item = section->items->first; item; item = item->next) {
        printf("[%d] %s\n", pos, item->item_name);
        pos += 1;
    }
}

static void show_all_sections(const struct section_dbl *sections)
{
    int pos;
    struct section_dbl_node *section;

    pos = 1;
    puts("==== Sections ====");
    for (section = sections->first; section; section = section->next) {
        printf("[%d] %s\n", pos, section->section_name);
        pos += 1;
    }
}

static struct item_dbl_node*
get_item(const char *item_name, struct section_dbl_node *section)
{
    struct item_dbl_node *item;
    int item_pos;

    item_pos = str_to_pos(item_name);
    item = item_dbl_search_by_pos(item_pos, section->items);
    if (!item)
        command_error("Incorrect item position [%d]\n", item_pos);

    return item;
}

static struct section_dbl_node*
get_section(const char *section_name, struct section_dbl *sections)
{
    struct section_dbl_node *section;
    if (isdigit(section_name[0])) {
        int section_pos = str_to_pos(section_name);
        section = section_dbl_search_by_pos(section_pos, sections);
        if (!section)
            command_error("Incorrect section position [%d]\n", section_pos);
    }
    else {
        section = section_dbl_search(section_name, sections);
    }

    return section;
}

static int add_item_cmd(const char *item_name, const char *section_name,
                         struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    sections = load_sections(cfg->data_location, cfg->default_section);
    if (!sections)
        return 1;

    section = get_section(section_name, sections);
    if (!section) {
        section_dbl_push_back(section_name, sections);
        section = sections->last;
    }
    
    item_dbl_push_back(item_name, section->items);
    put_sections(sections, cfg->default_section, cfg->data_location);

    show_section(section);

    section_dbl_free(sections);
    free(sections);

    return 0;
}

static int move_item_cmd(const char *item_name, const char *section_name,
                          const char *target_section_name, struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;
    struct section_dbl_node *target_section;

    sections = load_sections(cfg->data_location, cfg->default_section);
    if (!sections)
        return 1;
    section = get_section(section_name, sections);
    if (!section)
        command_error("Section \"%s\" not found\n", section_name);

    item = get_item(item_name, section);

    target_section = get_section(target_section_name, sections);
    if (!target_section) {
        section_dbl_push_back(target_section_name, sections);
        target_section = sections->last;
    }

    item_dbl_push_back(item->item_name, target_section->items);
    item_dbl_remove(item, section->items);

    if (item_dbl_is_empty(section->items))
        section_dbl_remove(section, sections);

    show_section(section);
    show_section(target_section);

    put_sections(sections, cfg->default_section, cfg->data_location);
    section_dbl_free(sections);
    free(sections);
    return 0;
}

static int copy_item_cmd(const char *item_name, const char *section_name,
                         const char *target_section_name, struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;
    struct section_dbl_node *target_section;

    sections = load_sections(cfg->data_location, cfg->default_section);
    if (!sections)
        return 1;
    section = get_section(section_name, sections);
    if (!section)
        command_error("Section \"%s\" not found\n", section_name);

    item = get_item(item_name, section);

    target_section = get_section(target_section_name, sections);
    if (!target_section) {
        section_dbl_push_back(target_section_name, sections);
        target_section = sections->last;
    }

    item_dbl_push_back(item->item_name, target_section->items);

    show_section(section);
    show_section(target_section);

    put_sections(sections, cfg->default_section, cfg->data_location);
    section_dbl_free(sections);
    free(sections);
    return 0;
}

static int delete_item_cmd(const char *item_name, const char *section_name, 
                           struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;

    sections = load_sections(cfg->data_location, cfg->default_section);
    if (!sections)
        return 1;

    section = get_section(section_name, sections);
    if (!section)
        command_error("Section \"%s\" not found\n", section_name);

    item = get_item(item_name, section);

    item_dbl_remove(item, section->items);
    if (item_dbl_is_empty(section->items) &&
            strcmp(section->section_name, cfg->default_section) != 0) 
    {
        printf("Section \"%s\" has been removed\n", section->section_name);
        section_dbl_remove(section, sections);
        show_all_sections(sections);;
    } else {
        show_section(section);
    }

    put_sections(sections, cfg->default_section, cfg->data_location);
    section_dbl_free(sections);
    free(sections);

    return 0;
}

static int show_section_cmd(const char *section_name, struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;

    sections = load_sections(cfg->data_location, cfg->default_section);
    if (!sections)
        return 1;

    section = get_section(section_name, sections);
    if (!section)
        command_error("Section \"%s\" not found\n", section_name);

    show_section(section);
    section_dbl_free(sections);
    free(sections);
    return 0;
}

static int show_all_sections_cmd(struct config *cfg)
{
    struct section_dbl *sections;

    sections = load_sections(cfg->data_location, cfg->default_section);
    if (!sections)
        return 1;
    show_all_sections(sections);
    section_dbl_free(sections);
    free(sections);
    return 0;
}

int perform_command(int argc, char **argv, struct config *cfg)
{
    int res;
    char *cmd;

    res = 0;
    switch (argc) {
    case 0:
        res = show_all_sections_cmd(cfg);
        break;
    case 1: 
        if (strcmp(argv[0], HELP_CMD) == 0) {
            printf(HELP);
        } else {
            res = show_section_cmd(argv[0], cfg);
        }
        break;
    default:
        cmd = argv[0];
        argc -= 1;
        argv += 1;

        if (strcmp(cmd, ADD_CMD) == 0) {
            if (argc >= 2) {
                res = add_item_cmd(argv[0], argv[1], cfg);
            } else {
                res = add_item_cmd(argv[0], cfg->default_section, cfg);
            }
        } else
        if (strcmp(cmd, MOVE_CMD) == 0) {
            if (argc >= 3)
                res = move_item_cmd(argv[0], argv[1], argv[2], cfg);
            else
                command_error("Too few arguments in command \"%s\"\n", cmd);
        } else
        if (strcmp(cmd, COPY_CMD) == 0) {
            if (argc >= 3)
                res = copy_item_cmd(argv[0], argv[1], argv[2], cfg);
            else
                command_error("Too few arguments in command \"%s\"\n", cmd);
        } else
        if (strcmp(cmd, DELETE_CMD) == 0) {
            if (argc >= 2)
                res = delete_item_cmd(argv[0], argv[1], cfg); 
            else
                command_error("Too few arguments in command \"%s\"\n", cmd);
        } else {
            command_error("Incorrect command \"%s\"\n", cmd);
        }
    }

    free(cfg);

    return res;
}
