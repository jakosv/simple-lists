#ifndef PARSER_H_SENTRY
#define PARSER_H_SENTRY

#include <stdio.h>

enum { max_parsed_value_len = 256 };
enum param_type { pt_none, pt_section, pt_task };
enum parser_error { 
    pe_too_long_section = 1, 
    pe_too_long_task, 
    pe_eof
};

struct file_parser {
    FILE *file;
    char parsed_value[max_parsed_value_len];
    enum param_type parsed_type; 
};

struct file_parser *parser_open(const char *filename);
void parser_free(struct file_parser *parser);
int parse_param(struct file_parser *parser);

#endif
