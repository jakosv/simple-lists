#include "file_parser.h"
#include "util.h"
#include "string.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum { 
    open_section_char = '[', 
    close_section_char = ']', 
    open_item_char = '-',
    end_line_item_char = '\n',
    multiline_start_char = ' '
};

struct file_parser *parser_open(const char *filename)
{
    struct file_parser *parser;
    parser = malloc(sizeof(*parser));
    parser->file = fopen(filename, "r");
    if (!parser->file)
        parser->file = fopen(filename, "w");
    parser->parsed_value = string_init();
    parser->parsed_type = pt_none; 
    parser->last_parsed_char = EOF;

    return parser;
}

void parser_free(struct file_parser *parser)
{
    fclose(parser->file);
    string_free(parser->parsed_value);
    parser->parsed_value = NULL;
    free(parser);
}

static int parse_line(struct file_parser *parser, char line_end_char)
{
    int ch; 
    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == line_end_char)
            return pe_ok;
        string_add_char(ch, parser->parsed_value); 
    }
    
    return pe_eof;
}

static int parse_item(struct file_parser *parser)
{
    int ch;

    do {
        parse_line(parser, end_line_item_char);
        ch = fgetc(parser->file);

        if (ch == multiline_start_char) {
            string_add_char(end_line_item_char, parser->parsed_value); 
            string_add_char(multiline_start_char, parser->parsed_value); 
        }
    } while (ch == multiline_start_char);

    parser->last_parsed_char = ch;
    if (parser->last_parsed_char == EOF)
        return pe_eof;

    return pe_ok;
}

int parse_param(struct file_parser *parser)
{
    int ch = parser->last_parsed_char;

    string_clear(parser->parsed_value);

    while (ch == open_section_char || ch == open_item_char ||
           (ch = fgetc(parser->file)) != EOF)
    {
        if (ch == open_section_char) {
            int res = parse_line(parser, close_section_char);
            if (res == pe_err)
                return res;
            parser->parsed_type = pt_section;
            return pe_ok;
        } else 
        if (ch == open_item_char) {
            int res = parse_item(parser); 
            if (res == pe_err)
                return res;
            parser->parsed_type = pt_task;
            return pe_ok;
        }
    }

    return pe_eof;
}

const char *get_parsed_value(struct file_parser *parser)
{
    const char *parsed_value;

    string_strip(parser->parsed_value);
    string_to_c_string(parser->parsed_value);
    parsed_value = get_string_data(parser->parsed_value);

    return parsed_value;
}

