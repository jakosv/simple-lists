#include "file_parser.h"
#include "section_dbl.h"

#include <stdio.h>

extern const char *unnamed_section_name;

void put_sections(struct section_dbl *sections, const char *filename);
struct section_dbl *load_sections(const char *filename);
