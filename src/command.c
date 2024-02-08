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

#define ADD_CMD "add"
#define MOVE_CMD "mv"
#define DELETE_CMD "del"

static void add_item_cmd(const char *item_name, const char *section_name,
                         struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    sections = load_sections(cfg->data_location, cfg->default_section);

    if (isdigit(section_name[0])) {
        section = section_dbl_search_by_pos(atoi(section_name), sections);
        if (!section) {
            fprintf(stderr, "Incorrect section position [%d]\n", 
                   atoi(section_name));
            exit(1);
        }
    }
    else {
        section = section_dbl_search(section_name, sections);
        if (!section) {
            section_dbl_push_back(section_name, sections);
            section = sections->last;
        }
    }
    item_dbl_push_back(item_name, section->items);
    put_sections(sections, cfg->default_section, cfg->data_location);
    section_dbl_free(sections);
    free(sections);
}

static void move_item_cmd(const char *item_name, const char *section_name,
                          const char *target_section_name, struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;
    struct section_dbl_node *target_section;

    sections = load_sections(cfg->data_location, cfg->default_section);

    if (isdigit(section_name[0])) {
        section = section_dbl_search_by_pos(atoi(section_name), sections);
        if (!section) {
            fprintf(stderr, "Incorrect section position [%d]\n", 
                    atoi(section_name));
            exit(1);
        }
    } else {
        section = section_dbl_search(section_name, sections);
        if (!section) {
            fprintf(stderr, "Section \"%s\" not found\n", section_name);
            exit(1);
        }
    }

    item = item_dbl_search_by_pos(atoi(item_name), section->items);
    if (!item) {
        fprintf(stderr, "Incorrect item position [%d]\n", atoi(item_name));
        exit(1);
    }

    if (isdigit(target_section_name[0])) {
        target_section = section_dbl_search_by_pos(atoi(target_section_name),
                                                   sections);
        if (!target_section) {
            fprintf(stderr, "Incorrect section position [%d]\n", 
                    atoi(target_section_name));
            exit(1);
        }
    } else {
        target_section = section_dbl_search(target_section_name, sections);
        if (!target_section) {
            section_dbl_push_back(target_section_name, sections);
            target_section = sections->last;
        }
    }

    item_dbl_push_back(item->item_name, target_section->items);
    item_dbl_remove(item, section->items);
    if (item_dbl_is_empty(section->items))
        section_dbl_remove(section, sections);
    target_section = section_dbl_search(target_section_name, sections);
    put_sections(sections, cfg->default_section, cfg->data_location);
    section_dbl_free(sections);
    free(sections);
}

static void delete_item_cmd(const char *item_name, const char *section_name, 
                            struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;

    sections = load_sections(cfg->data_location, cfg->default_section);

    if (isdigit(section_name[0])) {
        section = section_dbl_search_by_pos(atoi(section_name), sections);
        if (!section) {
            fprintf(stderr, "Incorrect section position [%d]\n", 
                    atoi(section_name));
            exit(1);
        }
    } else {
        section = section_dbl_search(section_name, sections);
        if (!section) {
            fprintf(stderr, "Section \"%s\" not found\n", section_name);
            exit(1);
        }
    }

    item = item_dbl_search_by_pos(atoi(item_name), section->items);
    if (!item) {
        fprintf(stderr, "Incorrect item position [%d]\n", atoi(item_name));
        exit(1);
    }

    item_dbl_remove(item, section->items);
    if (item_dbl_is_empty(section->items))
        section_dbl_remove(section, sections);

    put_sections(sections, cfg->default_section, cfg->data_location);
    section_dbl_free(sections);
    free(sections);
}

static void show_section_cmd(const char *section_name, struct config *cfg)
{
    int pos;
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;

    sections = load_sections(cfg->data_location, cfg->default_section);

    if (isdigit(section_name[0]))
        section = section_dbl_search_by_pos(atoi(section_name), sections);
    else
        section = section_dbl_search(section_name, sections);
    if (!section) {
        fprintf(stderr, "Section \"%s\" not found\n", section_name);
        exit(1);
    }

    printf("==== %s ====\n", section->section_name);
    pos = 1;
    for (item = section->items->first; item; item = item->next) {
        printf("[%d] %s\n", pos, item->item_name);
        pos += 1;
    }
    section_dbl_free(sections);
    free(sections);
}

static void show_all_sections_cmd(struct config *cfg)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    int pos;

    pos = 1;
    sections = load_sections(cfg->data_location, cfg->default_section);
    puts("==== Sections ====");
    for (section = sections->first; section; section = section->next) {
        printf("[%d] %s\n", pos, section->section_name);
        pos += 1;
    }
    section_dbl_free(sections);
    free(sections);
}

void perform_command(int argc, char **argv, struct config *cfg)
{
    char *cmd;
    switch (argc) {
    case 0:
        show_all_sections_cmd(cfg);
        break;
    case 1: 
        show_section_cmd(argv[0], cfg);
        break;
    default:
        cmd = argv[0];
        argc -= 1;
        argv += 1;

        if (strcmp(cmd, ADD_CMD) == 0) {
            puts(argv[0]);
            if (argc > 1) {
                puts(argv[1]);
                add_item_cmd(argv[0], argv[1], cfg);
                show_section_cmd(argv[1], cfg);
            } else {
                add_item_cmd(argv[0], cfg->default_section, cfg);
                show_section_cmd(cfg->default_section, cfg);
            }
        } else
        if (strcmp(cmd, MOVE_CMD) == 0) {
            move_item_cmd(argv[0], argv[1], argv[2], cfg);
            show_section_cmd(argv[1], cfg);
            show_section_cmd(argv[2], cfg);
        } else
        if (strcmp(cmd, DELETE_CMD) == 0) {
            delete_item_cmd(argv[0], argv[1], cfg); 
            show_section_cmd(argv[1], cfg);
        }
    }
    free(cfg);
}
