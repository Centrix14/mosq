#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st.h/st.h"
#include "tl2/list.h"
#include "pars.h"
#include "list_builder.h"
#include "interp.h"
#include "token.h"

list *programm = NULL;
list *target_node = NULL;

void lb_init() {
	programm = list_init_node(NULL);
}

void lb_free() {
	list_free_list(programm);
}

void lb_add_token(pl_state *ps) {
	char *data;
	list *last;
	token *tok;

	data = ps->buffer;
	list_add_node(programm);

	last = list_get_last(programm);
	
	tok = tl_alloc(data);
	if (!tok) {
		st_err("fail to allocate memory");
	}

	list_set_data(last, tok);
}

void lb_show_list() {
	token *tok;
	list *lptr;
	int i = 0;

	lptr = programm;
	while (lptr) {
		if (lptr->data) {
			tok = (token*)lptr->data;
			printf("[%3d]: val=%s\tret=%s\n", i++, (char*)tok->val, (char*)tok->ret);
			//printf("%s ", (char*)tok->val);
		}

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
	token *tk = NULL;

	addr = tok;
	if (*tok == '@')
		addr = &tok[1];

	lptr = node;
	tk = (token*)lptr->data;
	data = tk->val;

	while (lptr) {
		if (data && !strcmp(addr, data)) // tok == data
			break;

		lptr = lptr->next;

		tk = (token*)lptr->data;
		data = tk->val;
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
	token *tk = NULL;

	lptr = node->next;
	tok = strtok(addr, ":");
	while (tok) {
		lptr = eval_addr_tok(lptr, tok);

		tok = strtok(NULL, ":");
	}

	if (lptr && lptr->data) {
		target_node = lptr;

		tk = (token*)lptr->data;
		return tk->ret;
	}
	return NULL;
}

void lb_insert_in_list(list *lptr, char *val, char *ret) {
	list *new = NULL;
	token *tok = NULL;

	new = list_init_node(lptr);
	
	new->next = lptr->next;
	lptr->next = new;
	lptr->next->prev = new;

	tok = tl_alloc(val);
	tl_set_ret(tok, ret);
	new->data = tok;
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
	token *tok = NULL;
	char *data = NULL;
	int brackets = 1;

	if (!lptr->next)
		return;

	node = lptr->next;
	if (!node->data)
		return ;

	tok = (token*)node->data;
	data = (char*)tok->val;
	while (node) {
		next = node->next;

		if (!data)
			break;

		if (!strcmp(data, "["))
			brackets++;
		else if (!strcmp(data, "]")) {
			brackets--;

			if (!brackets)
				break;
		}

		il_eval(node);

		node = next;
		tok = (token*)node->data;
		data = (char*)tok->val;
	}
}
