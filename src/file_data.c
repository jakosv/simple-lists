#include "file_data.h"
#include "file_parser.h"
#include "section_dbl.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void put_items(struct item_dbl *items, FILE *file)
{
    struct item_dbl_node *tmp;
    for (tmp = items->first; tmp; tmp = tmp->next)
        fprintf(file, "- %s\n", tmp->item_name);
}

void put_sections(struct section_dbl *sections, const char *default_section,
                  const char *filename)
{
    FILE *file;
    struct section_dbl_node *tmp;
    file = fopen(filename, "w");
    if (!file) {
        perror(filename);
        exit(1);
    }
    for (tmp = sections->first; tmp; tmp = tmp->next) {
        if (strcmp(tmp->section_name, default_section) != 0)
            fprintf(file, "[%s]\n", tmp->section_name);
        put_items(tmp->items, file);    
        if (tmp->next)
            fputc('\n', file);
    }
    fclose(file);
}

struct section_dbl *load_sections(const char *filename, 
                                  const char *default_section)
{
    struct file_parser *parser;
    struct section_dbl_node *section_node;
    struct section_dbl *sections;
    int res;

    sections = malloc(sizeof(*sections));
    section_dbl_init(sections);

    parser = parser_open(filename);
    if (!parser)
        return sections;

    section_dbl_push_back(default_section, sections);
    section_node = sections->last;

    while ((res = parse_param(parser)) != pe_eof) {
        const char *parsed_value;

        parsed_value = get_parsed_value(parser);
        if (parser->parsed_type == pt_section) {
            section_dbl_push_back(parsed_value, sections);
            section_node = sections->last;
        } else {
            item_dbl_push_back(parsed_value, section_node->items);
        }
    }

    parser_free(parser);
    return sections;
}
