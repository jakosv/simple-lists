#include "file_parser.h"
#include "section_dbl.h"
#include "item_dbl.h"
#include "file_data.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

void add_item_cmd(const char *item_name, const char *section_name,
                  const char *filename)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    sections = load_sections(filename);

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
    put_sections(sections, filename);
}

void move_item_cmd(const char *item_name, const char *section_name,
                   const char *target_section_name, const char *filename)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;
    struct section_dbl_node *target_section;

    sections = load_sections(filename);

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
    target_section = section_dbl_search(target_section_name, sections);
    put_sections(sections, filename);
}

void delete_item_cmd(const char *item_name, const char *section_name, 
                     const char *filename)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;

    sections = load_sections(filename);

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

    item_dbl_remove(item, section->items);

    put_sections(sections, filename);
}

void show_section_cmd(const char *section_name, const char *filename)
{
    int pos;
    struct section_dbl *sections;
    struct section_dbl_node *section;
    struct item_dbl_node *item;

    sections = load_sections(filename);

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
}

void show_all_sections_cmd(const char *filename)
{
    struct section_dbl *sections;
    struct section_dbl_node *section;
    int pos;

    pos = 0;
    sections = load_sections(filename);
    puts("==== Sections ====");
    for (section = sections->first; section; section = section->next) {
        struct item_dbl_node *item;
        printf("[%d] %s\n", pos, section->section_name);
        pos += 1;
        /*
        for (item = section->items->first; item; item = item->next)
            printf("- %s\n", item->item_name);
        */
    }
}

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
    int ch;
    char *cmd = NULL;
    char *filename = NULL;
    char *item = NULL;
    char *section = NULL;
    char *target_section = NULL;

    cmd = argv[1];
    optind = 2;

    while ((ch = getopt(argc, argv, "f:i:s:t:")) != -1)  {
        switch (ch) {
        case 'f':
            filename = get_option_copy(optarg);
            break;
        case 'i':
            item = get_option_copy(optarg);
            break;
        case 's':
            section = get_option_copy(optarg);
            break;
        case 't':
            target_section = get_option_copy(optarg);
            break;
        default:
            break;
        }
    }

    if (strcmp(cmd, "add") == 0) {
        if (section)
            add_item_cmd(item, section, filename);
        else
            add_item_cmd(item, unnamed_section_name, filename);
    } else
    if (strcmp(cmd, "show") == 0) {
        if (section)
            show_section_cmd(section, filename);
        else
            show_all_sections_cmd(filename);
    } else
    if (strcmp(cmd, "move") == 0) {
        move_item_cmd(item, section, target_section, filename);
    } else
    if (strcmp(cmd, "delete") == 0) {
        delete_item_cmd(item, section, filename); 
    }

    free(item);
    free(section);
    free(target_section);
    free(filename);

    return 0;
}
