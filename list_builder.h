#ifndef list_builder
#define list_builder

#include "tl2/list.h"
#include "pars.h"

void lb_init();
void lb_free();

void lb_add_token(pl_state *ps);
void lb_add_block(pl_state *ps);

void lb_show_list();

void lb_crawl(void (*func)(list*));

void lb_print_node(list *node);

char *lb_eval_addr(char *addr, list *node);

int lb_is_addr_expr(char *expr);

void lb_insert_in_list(list *lptr, char *str);

void lb_expand_addr(list *node, char *addr);

#endif
