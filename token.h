#ifndef token_h
#define token_h

#include "tl2/list.h"

typedef struct {
	char *val;
	char *ret;
} token;

void tl_free(token *tok);
void tl_free_list(list *node);

token *tl_alloc(char *buf);

#endif
