#include "file_parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct file_parser *parser_open(const char *filename)
{
    struct file_parser *parser;
    parser = malloc(sizeof(*parser));
    parser->file = fopen(filename, "r");
    if (!parser->file) {
        perror(filename);
        return NULL;
    }
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
        if (ch == ']') {
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
        if (ch == '\n' && !was_backslash) {
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

static void str_strip(char *str)
{
    char *s;
    int i, len;

    len = strlen(str);
    s = str;
    /* skip leading spaces */
    for (; *s == ' '; s++)
        len--;

    /* skip trailing spaces */
    for (i = len-1; i >= 0; i--)
        if (s[i] != ' ')
            break;
    len = i + 1;

    for (i = 0; i < len; i++)
        str[i] = s[i];
    str[len] = '\0';
}

int parse_param(struct file_parser *parser)
{
    int ch;
    while ((ch = fgetc(parser->file)) != EOF) {
        if (ch == '[') {
            int res = parse_section(parser);
            if (res)
                return res;
            parser->parsed_type = pt_section;
            str_strip(parser->parsed_value);
            return 0;
        } else 
        if (ch == '-') {
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

