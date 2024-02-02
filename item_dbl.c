#include "item_dbl.h"

#include <stdlib.h>
#include <string.h>

void item_dbl_init(struct item_dbl *lst)
{
    lst->first = NULL;
    lst->last = NULL;
}

static void free_node(struct item_dbl_node *node)
{
    free(node->item_name);
    free(node);
}

void item_dbl_free(struct item_dbl *lst)
{
    struct item_dbl_node *node;
    struct item_dbl_node *tmp;
    node = lst->first;
    while (node) {
        tmp = node;
        node = node->next;
        free_node(tmp);
    }
}

static void item_dbl_node_init(struct item_dbl_node *node,
                               const char *item_name)
{
    int name_len;

    name_len = strlen(item_name);
    node->item_name = malloc(name_len + 1);
    strcpy(node->item_name, item_name);
    node->item_name[name_len] = '\0';

    node->next = NULL;
    node->prev = NULL;
}

void item_dbl_push_front(const char *item_name, struct item_dbl *lst)
{
    struct item_dbl_node *tmp;
    
    tmp = malloc(sizeof(*tmp));
    item_dbl_node_init(tmp, item_name);

    tmp->next = lst->first;
    if (lst->last)
        lst->first->prev = tmp;
    else
        lst->last = tmp;
    lst->first = tmp;
}

void item_dbl_push_back(const char *item_name, struct item_dbl *lst)
{
    struct item_dbl_node *tmp;
    
    tmp = malloc(sizeof(*tmp));
    item_dbl_node_init(tmp, item_name);

    tmp->prev = lst->last;
    if (lst->first)
        lst->last->next = tmp;
    else
        lst->first = tmp;
    lst->last = tmp;
}

void item_dbl_remove(struct item_dbl_node *node, struct item_dbl *lst)
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

struct item_dbl_node *item_dbl_search_by_pos(int pos, struct item_dbl *lst)
{
    int cnt;
    struct item_dbl_node *tmp;
    cnt = 1;
    for (tmp = lst->first; tmp; tmp = tmp->next) {
        if (cnt == pos)
            return tmp;
        cnt += 1;
    }
    return NULL;
}
