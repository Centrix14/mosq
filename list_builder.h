#ifndef list_builder
#define list_builder

#include "tl2/list.h"
#include "pars.h"

void lb_init();
void lb_free();

void lb_add_token(pl_state *ps);
void lb_insert_in_list(list *lptr, char *val, char *ret);

void lb_show_list();
void lb_print_node(list *node);

void lb_crawl(void (*func)(list*));

char *lb_eval_addr(char *addr, list *node);
int lb_is_addr_expr(char *expr);
void lb_expand_addr(list *node, char *addr);
void lb_eval_block(list *lptr);

#endif
