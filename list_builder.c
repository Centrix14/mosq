#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl2/list.h"
#include "pars.h"
#include "list_builder.h"
#include "interp.h"

list *programm = NULL;
list *target_node = NULL;

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
		if (lptr->data)
			//printf("[%3d]: %s\n", i++, (char*)lptr->data);
			printf("%s ", (char*)lptr->data);

		lptr = lptr->next;
	}
}

void lb_crawl(void (*func)(list*)) {
	list *lptr, *next;

	lptr = programm;
	while (lptr) {
		next = lptr->next;

		if (lptr->data)
			(*func)(lptr);

		lptr = next;
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

	if (!lptr)
		return NULL;

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

	if (lptr && lptr->data) {
		target_node = lptr;

		return lptr->data;
	}
	return NULL;
}

void lb_insert_in_list(list *lptr, char *str) {
	list *new = NULL;

	new = list_init_node(lptr);
	
	new->next = lptr->next;
	lptr->next = new;
	lptr->next->prev = new;

	new->data = malloc(strlen(str) + 1);
	strcpy(new->data, str);
}

void __lb_delete_addr(list *node) {
	list *node_prev, *node_next;

	node_prev = node->prev;
	node_next = node->next;

	if (node_prev)
		node_prev->next = node_next;
	if (node_next)
		node_next->prev = node_prev;

	if (node->data)
		free(node->data);
	free(node);
}

void lb_expand_block(list *node, char *addr) {
	list *lptr = NULL, *addr_list = NULL;
	char *lptr_data = NULL, *addr_list_data = NULL;
	int brackets = 0;

	//lptr = node->next;
	lptr = node;
	if (!lptr)
		return ;

	lb_eval_addr(addr, programm);
	addr_list = target_node->next;
	addr_list_data = addr_list->data;

	while (brackets || (strcmp(addr_list_data, "]") && !brackets)) {
		if (!strcmp(addr_list_data, "["))
			brackets++;
		else if (!strcmp(addr_list_data, "]"))
			brackets--;

		lb_insert_in_list(lptr, addr_list_data);

		lptr = lptr->next;
		addr_list = addr_list->next;
		addr_list_data = (addr_list) ? addr_list->data : NULL;

		if (!addr_list_data)
			continue;
	}
}

void lb_expand_addr(list *node, char *addr) {
	char *data = NULL, *addr_copy;

	// copy addr to data
	addr_copy = pl_alloc_buf(strlen(addr) + 1);
	strcpy(addr_copy, addr);

	data = lb_eval_addr(addr_copy, programm);
	if (!data)
		return ;
	else if (!strcmp(data, "["))
		lb_eval_block(target_node);
	else
		lb_insert_in_list(node, data);
	
	__lb_delete_addr(node);
}

void lb_eval_block(list *lptr) {
	list *node = NULL, *next = NULL;
	int brackets = 1;

	if (!lptr->next)
		return;

	node = lptr->next;
	while (node) {
		next = node->next;

		if (!node->data)
			break;

		if (!strcmp(node->data, "["))
			brackets++;
		else if (!strcmp(node->data, "]")) {
			brackets--;

			if (!brackets)
				break;
		}

		il_eval(node);
		node = next;
	}
}
