#include "file_parser.h"
#include "section_dbl.h"
#include "config.h"

#include <stdio.h>

#define DATA_RONLY "r"
#define DATA_RW "rw"

extern const char *unnamed_section_name;

void put_sections(struct section_dbl *sections, const char *default_section,
                  const char *filename);
struct section_dbl *load_sections(const char *filename, 
                                  const char *default_section,
                                  const char *access);
