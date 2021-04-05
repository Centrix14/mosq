#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "tl2/list.h"

void tl_free(token *tok) {
	if (!tok)
		return;

	if (tok->val)
		free(tok->val);
	if (tok->ret)
		free(tok->ret);
	free(tok);
}

token *tl_alloc(char *buf) {
	token *tok = NULL;

	tok = (token*)malloc(sizeof(token));
	if (!tok)
		return NULL;

	tok->val = (char*)malloc(strlen(buf));
	tok->ret = (char*)malloc(strlen(buf));

	strcpy(tok->val, buf);
	strcpy(tok->ret, buf);

	return tok;
}

void tl_set_ret(token *tok, char *buf) {
	if (!tok)
		return ;
	if (!tok->ret)
		tok->ret = (char*)malloc(strlen(buf));
	else
		tok->ret = (char*)realloc(tok->ret, strlen(buf));

	strcpy(tok->ret, buf);
}

void tl_free_list(list *node) {
	if (node && node->data)
		tl_free((token*)node->data);
}
