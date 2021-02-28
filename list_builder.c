#include <stdio.h>
#include <string.h>

#include "tl2/list.h"
#include "pars.h"
#include "list_builder.h"

list *programm = NULL;

void lb_init() {
	programm = list_init_node(NULL);
}

void lb_free() {
	list_free_list(programm);
}

void lb_add_token(pl_state *ps) {
	list *last;
	char *node_buf;

	list_add_node(programm);

	last = list_get_last(programm);
	
	node_buf = pl_alloc_buf(strlen(ps->buffer) + 1);
	strcpy(node_buf, ps->buffer);
	list_set_data(last, node_buf);
}

void lb_show_list() {
	list *lptr;
	int i = 0;

	lptr = programm;
	while (lptr) {
		printf("[%3d]: %s\n", i++, (char*)lptr->data);

		lptr = lptr->next;
	}
}

void lb_crawl(void (*func)(list*)) {
	list *lptr;

	lptr = programm;
	while (lptr) {
		if (lptr->data)
			(*func)(lptr);

		lptr = lptr->next;
	}
}

void lb_print_node(list *node) {
	printf("%s\n", (char*)node->data);
}
