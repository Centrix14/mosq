#include <stdio.h>
#include <string.h>

#include "tl2/list.h"
#include "list_builder.h"
#include "interp.h"
#include "token.h"

char *std_func_list[] = {"print", "help", "reval", "input", NULL};
void (*std_cb_list[])(list*) = {il_cb_print, il_cb_help, il_cb_reval, il_cb_input};

void il_eval(list *node) {
	static int is_block = 0; 
	token *tk = NULL;
	char *data;

	tk = (token*)node->data;
	data = (char*)tk->val;
	is_block += il_is_block(data);

	if (lb_is_addr_expr(data))
		lb_expand_addr(node, data);
	else if (!is_block)
		il_eval_as_func(data, node);
}

int il_is_block(char *tok) {
	if (*tok == '[')
		return 1;
	else if (*tok == ']')
		return -1;
	return 0;
}

int il_is_std_func(char *tok) {
	for (int i = 0; std_func_list[i]; i++)
		if (!strcmp(std_func_list[i], tok))
			return i;
	return -1;
}

void il_eval_as_func(char *tok, list *node) {
	int res = -1;

	res = il_is_std_func(tok);
	if (res > -1)
		il_run(res, node);
}

void il_run(int code, list *node) {
	(*std_cb_list[code])(node);
}

void il_expand(list *node) {
	char *data = NULL;
	token *tok = NULL;

	tok = (token*)node->data;
	data = (char*)tok->val;
	if (lb_is_addr_expr(data))
		lb_expand_addr(node, data);
}

int __il_is_esc_seq(char *str) {
	char *esc[] = {".n", ".t", ".p", NULL};

	for (int i = 0; esc[i]; i++)
		if (!strcmp(esc[i], str))
			return i;
	return -1;
}

void __il_print_esc(char *str) {
	if (!strcmp(str, ".n"))
		putc('\n', stdout);
	else if (!strcmp(str, ".p"))
		lb_show_list();
	else
		putc('\t', stdout);
}

void __il_print_list(list *node) {
	token *tok;
	list *lptr;
	char *str;

	lptr = node;
	while (lptr) {
		if (!lptr->data) {
			lptr = lptr->next;

			continue;
		}

		tok = (token*)lptr->data;
		str = (char*)tok->ret;
		if (!strcmp(str, "]")) // str == "]"
			return ;
		else if (__il_is_esc_seq(str) > -1)
			__il_print_esc(str);
		else
			printf("%s", str);

		lptr = lptr->next;
	}
}

void il_cb_print(list *node) {
	token *tok;
	list *arg;
	char *str;

	arg = node->next;
	if (!arg || !arg->data)
		return ;

	tok = (token*)arg->data;
	str = (char*)tok->ret;

	if (strcmp(str, "[")) { // str != [
		if (__il_is_esc_seq(str) > -1)
			__il_print_esc(str);
		else
			printf("%s", str);
	}
	else
		__il_print_list(arg->next);
}

void il_cb_help(list *node) {
	printf("\n\tIt\'s MI - MOSQ Interpreter v0.1\n\t22.02.2021 by Centrix\n\n");
}

void il_cb_reval(list *node) {
	lb_crawl(il_eval);
}

void il_cb_input(list *node) {
	char buf[256] = "";

	fgets(buf, 256, stdin);
	if (strlen(buf) > 0)
		lb_insert_in_list(node, buf);
}
