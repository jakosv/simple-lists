#include "file_parser.h"
#include "util.h"
#include "parsed_str.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum { 
    open_section_char = '[', 
    close_section_char = ']', 
    open_item_char = '-',
    close_item_char = '\n'
};

struct file_parser *parser_open(const char *filename)
{
    struct file_parser *parser;
    parser = malloc(sizeof(*parser));
    parser->file = fopen(filename, "r");
    if (!parser->file)
        parser->file = fopen(filename, "w");
    parser->parsed_value = parsed_str_init();
    parser->parsed_type = pt_none; 

    return parser;
}

void parser_free(struct file_parser *parser)
{
    fclose(parser->file);
    parsed_str_free(parser->parsed_value);
    parser->parsed_value = NULL;
    free(parser);
}

static int parse_section(struct file_parser *parser)
{
    int ch; 
    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == close_section_char)
            return 0;
        parsed_str_add_char(ch, parser->parsed_value); 
    }
    
    return pe_eof;
}

static int parse_item(struct file_parser *parser)
{
    int ch, was_backslash; 
    was_backslash = 0;
    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == close_item_char && !was_backslash)
            return 0;
        if (was_backslash)
            was_backslash = 0;
        if (ch == '\\') {
            was_backslash = 1;
            continue;
        }
        parsed_str_add_char(ch, parser->parsed_value); 
    }

    return pe_eof;
}

int parse_param(struct file_parser *parser)
{
    int ch;

    parsed_str_clear(parser->parsed_value);

    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == open_section_char) {
            int res = parse_section(parser);
            if (res)
                return res;
            parser->parsed_type = pt_section;
            return 0;
        } else 
        if (ch == open_item_char) {
            int res = parse_item(parser); 
            if (res)
                return res;
            parser->parsed_type = pt_task;
            return 0;
        }
    }
    return pe_eof;
}

const char *get_parsed_value(struct file_parser *parser)
{
    const char *parsed_value;

    parsed_str_strip(parser->parsed_value);
    parsed_str_to_c_string(parser->parsed_value);
    parsed_value = get_parsed_str_data(parser->parsed_value);

    return parsed_value;
}

