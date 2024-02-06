#include "file_parser.h"
#include "util.h"

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
    parser->parsed_type = pt_none; 

    return parser;
}

void parser_free(struct file_parser *parser)
{
    fclose(parser->file);
    free(parser);
}

static int parse_section(struct file_parser *parser)
{
    int ch, value_len; 
    value_len = 0;
    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == close_section_char) {
            parser->parsed_value[value_len] = 0; 
            return 0;
        }
        parser->parsed_value[value_len] = ch; 
        value_len++;
        if (value_len == max_parsed_value_len-1) {
            parser->parsed_value[value_len] = 0; 
            return pe_too_long_section;
        }
    }
    
    return pe_eof;
}

static int parse_item(struct file_parser *parser)
{
    int ch, value_len, was_backslash; 
    was_backslash = 0;
    value_len = 0;
    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == close_item_char && !was_backslash) {
            parser->parsed_value[value_len] = 0; 
            return 0;
        }
        if (was_backslash)
            was_backslash = 0;
        if (ch == '\\') {
            was_backslash = 1;
            continue;
        }
        parser->parsed_value[value_len] = ch; 
        value_len++;
        if (value_len == max_parsed_value_len-1) {
            parser->parsed_value[value_len] = 0; 
            return pe_too_long_task;
        }
    }

    parser->parsed_value[value_len] = 0; 
    return pe_eof;
}

int parse_param(struct file_parser *parser)
{
    int ch;
    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == open_section_char) {
            int res = parse_section(parser);
            if (res)
                return res;
            parser->parsed_type = pt_section;
            str_strip(parser->parsed_value);
            return 0;
        } else 
        if (ch == open_item_char) {
            int res = parse_item(parser); 
            if (res)
                return res;
            parser->parsed_type = pt_task;
            str_strip(parser->parsed_value);
            return 0;
        }
    }
    return pe_eof;
}

