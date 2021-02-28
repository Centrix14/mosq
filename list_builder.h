#ifndef list_builder
#define list_builder

#include "tl2/list.h"
#include "pars.h"

void lb_init();
void lb_free();

void lb_add_token(pl_state *ps);
void lb_show_list();

void lb_crawl(void (*func)(list*));

void lb_print_node(list *node);

#endif
