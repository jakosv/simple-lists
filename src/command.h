#ifndef COMMAND_H_SENTRY
#define COMMAND_H_SENTRY

#include "config.h"

void add_item_cmd(const char *item_name, const char *section_name,
                  struct config *cfg);
void move_item_cmd(const char *item_name, const char *section_name,
                   const char *target_section_name, struct config *cfg);
void delete_item_cmd(const char *item_name, const char *section_name, 
                     struct config *cfg);
void show_section_cmd(const char *section_name, struct config *cfg);
void show_all_sections_cmd(struct config *cfg);

#endif
