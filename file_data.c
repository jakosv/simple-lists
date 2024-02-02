#include "file_data.h"
#include "file_parser.h"
#include "section_dbl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *unnamed_section_name = "without section";

static void put_section(struct section_dbl_node *section_node, FILE* file)
{
    struct item_dbl_node *tmp;
    if (strcmp(section_node->section_name, unnamed_section_name) != 0)
        fprintf(file, "[%s]\n", section_node->section_name);
    for (tmp = section_node->items->first; tmp; tmp = tmp->next)
        fprintf(file, "- %s\n", tmp->item_name);
}

void put_sections(struct section_dbl *sections, const char *filename)
{
    FILE *file;
    struct section_dbl_node *tmp;
    file = fopen(filename, "w");
    if (!file) {
        perror(filename);
        exit(1);
    }
    for (tmp = sections->first; tmp; tmp = tmp->next) {
        put_section(tmp, file);    
        if (tmp->next)
            fputc('\n', file);
    }
    fclose(file);
}

struct section_dbl *load_sections(const char *filename)
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

    section_dbl_push_back(unnamed_section_name, sections);
    section_node = sections->last;

    while ((res = parse_param(parser)) != pe_eof) {
        if (res > 0) {
            switch (res) {
            case pe_too_long_section:
                fprintf(stderr, "Parse error: too long section\n");
                break;
            case pe_too_long_task:
                fprintf(stderr, "Parse error: too long task\n");
                break;
            }
            return NULL;
        }
        if (parser->parsed_type == pt_section) {
            section_dbl_push_back(parser->parsed_value, sections);
            section_node = sections->last;
        } else {
            item_dbl_push_back(parser->parsed_value, section_node->items);
        }
    }
    parser_free(parser);
    return sections;
}
