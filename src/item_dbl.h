#ifndef ITEM_DBL_H_SENTRY
#define ITEM_DBL_H_SENTRY

#include "item_dbl.h"

struct item_dbl_node {
    char *item_name;
    struct item_dbl_node *next;
    struct item_dbl_node *prev;
};

struct item_dbl {
    struct item_dbl_node *first;
    struct item_dbl_node *last;
};

void item_dbl_init(struct item_dbl *lst);
void item_dbl_free(struct item_dbl *lst);
void item_dbl_push_front(const char *item_name, struct item_dbl *lst);
void item_dbl_push_back(const char *item_name, struct item_dbl *lst);
void item_dbl_remove(struct item_dbl_node *node, struct item_dbl *lst);
struct item_dbl_node *item_dbl_search_by_pos(int pos, struct item_dbl *lst);

#endif
