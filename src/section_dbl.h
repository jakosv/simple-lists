#ifndef SECTION_DBL_H_SENTRY
#define SECTION_DBL_H_SENTRY

#include "item_dbl.h"

struct section_dbl_node {
    char *section_name;
    struct item_dbl *items;
    struct section_dbl_node *next;
    struct section_dbl_node *prev;
};

struct section_dbl {
    struct section_dbl_node *first;
    struct section_dbl_node *last;
};

void section_dbl_init(struct section_dbl *lst);
void section_dbl_free(struct section_dbl *lst);
void section_dbl_push_front(const char *section_name, 
                            struct section_dbl *lst);
void section_dbl_push_back(const char *section_name, struct section_dbl *lst);
void section_dbl_remove(struct section_dbl_node *node, 
                        struct section_dbl *lst);
struct section_dbl_node*
section_dbl_search(const char *section_name, struct section_dbl *lst);
struct section_dbl_node*
section_dbl_search_by_pos(int pos, struct section_dbl *lst);

#endif
