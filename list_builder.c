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
	char *node_buf, *data;
	list *last;

	data = ps->buffer;
	if (lb_is_addr_expr(data))
		data = lb_eval_addr(data, programm);

	list_add_node(programm);

	last = list_get_last(programm);
	
	node_buf = pl_alloc_buf(strlen(data) + 1);
	strcpy(node_buf, data);
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

int lb_is_addr_expr(char *expr) {
	if (*expr == '@')
		return 1;
	else if (strchr(expr, ':'))
		return 1;
	return 0;
}

list *__list_get_first(list *node) {
	list *lptr = NULL;

	lptr = node;
	while (lptr->prev)
		lptr = lptr->prev;
	return lptr;
}

list *eval_addr_tok(list *node, char *tok) {
	char *data = NULL, *addr = NULL;
	list *lptr = NULL;

	addr = tok;
	if (*tok == '@')
		addr = &tok[1];

	lptr = node;
	data = (char*)list_get_data(lptr);
	while (lptr) {
		if (data && !strcmp(addr, data)) // tok == data
			break;

		lptr = lptr->next;

		data = (char*)list_get_data(lptr);
	}

	if (*tok == '@')
		return lptr->next;
	return lptr;
}

char *lb_eval_addr(char *addr, list *node) {
	char *tok = NULL;
	list *lptr = NULL;

	lptr = node->next;
	tok = strtok(addr, ":");
	while (tok) {
		lptr = eval_addr_tok(lptr, tok);

		tok = strtok(NULL, ":");
	}

	if (lptr && lptr->data)
		return lptr->data;
	return NULL;
}
