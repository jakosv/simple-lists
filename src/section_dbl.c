#include "section_dbl.h"
#include "item_dbl.h"

#include <stdlib.h>
#include <string.h>

void section_dbl_init(struct section_dbl *lst)
{
    lst->first = NULL;
    lst->last = NULL; 
}

static void free_node(struct section_dbl_node *node)
{
    free(node->section_name);
    item_dbl_free(node->items);
    free(node);
}

void section_dbl_free(struct section_dbl *lst)
{
    struct section_dbl_node *tmp;
    struct section_dbl_node *node;
    node = lst->first;
    while (node) {
        tmp = node;
        node = node->next;
        free_node(tmp);
    }
    lst->first = NULL;
    lst->last = NULL; 
}

static void section_dbl_init_node(struct section_dbl_node *node,
                                  const char *section_name)
{
    int name_len;
    name_len = strlen(section_name);
    node->section_name = malloc(name_len + 1);
    strcpy(node->section_name, section_name);
    node->section_name[name_len] = '\0';

    node->items = malloc(sizeof(struct item_dbl));
    item_dbl_init(node->items);
    node->next = NULL;
    node->prev = NULL;
}

void section_dbl_push_front(const char *section_name, struct section_dbl *lst)
{
    struct section_dbl_node *tmp;

    tmp = malloc(sizeof(*tmp));
    section_dbl_init_node(tmp, section_name);

    tmp->next = lst->first;
    if (lst->last)
        lst->first->prev = tmp;
    else
        lst->last = tmp;
    lst->first = tmp;
}

void section_dbl_push_back(const char *section_name, struct section_dbl *lst)
{
    struct section_dbl_node *tmp;

    tmp = malloc(sizeof(*tmp));
    section_dbl_init_node(tmp, section_name);

    tmp->prev = lst->last;
    if (lst->first)
        lst->last->next = tmp;
    else
        lst->first = tmp;
    lst->last = tmp;
}

void section_dbl_remove(struct section_dbl_node *node, struct section_dbl *lst)
{
    if (node->prev)
        node->prev->next = node->next; 
    else
        lst->first = node->next;

    if (node->next)
        node->next->prev = node->prev;
    else
        lst->last = node->prev;

    free_node(node);
}

struct section_dbl_node*
section_dbl_search(const char *section_name, struct section_dbl *lst)
{
    struct section_dbl_node *tmp;
    for (tmp = lst->first; tmp; tmp = tmp->next)
        if (strcmp(tmp->section_name, section_name) == 0)
            return tmp;
    return NULL;
}

struct section_dbl_node*
section_dbl_search_by_pos(int pos, struct section_dbl *lst)
{
    struct section_dbl_node *tmp;
    int cnt;

    cnt = 1;
    for (tmp = lst->first; tmp; tmp = tmp->next) {
        if (cnt == pos)
            return tmp;
        cnt += 1;
    }

    return NULL;
}
