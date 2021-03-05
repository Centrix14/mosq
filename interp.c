#include <stdio.h>
#include <string.h>

#include "tl2/list.h"
#include "list_builder.h"
#include "interp.h"

char *std_func_list[] = {"print", "help", "reval", NULL};
void (*std_cb_list[])(list*) = {il_cb_print, il_cb_help, il_cb_reval};

void il_eval(list *node) {
	static int is_block = 0; 
	char *data;

	data = (char*)node->data;
	if (il_is_block(data))
		is_block = !is_block;
	else if (!is_block)
		il_eval_as_func(data, node);
}

int il_is_block(char *tok) {
	if (strstr("[]", tok))
		return 1;
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
	list *lptr;
	char *str;

	lptr = node;
	while (lptr) {
		if (!lptr->data) {
			lptr = lptr->next;

			continue;
		}

		str = (char*)lptr->data;
		if (!strcmp(str, "]")) // str == "]"
			return ;
		else if (__il_is_esc_seq(str) > -1)
			__il_print_esc(str);
		else
			printf("%s ", str);

		lptr = lptr->next;
	}
}

void il_cb_print(list *node) {
	list *arg;
	char *str;

	arg = node->next;
	if (arg && arg->data)
		str = (char*)arg->data;

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
