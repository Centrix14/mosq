#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "st.h/st.h"
#include "pars.h"
#include "list_builder.h"
#include "interp.h"

int main() {
	lb_init();

	pl_open_file("code.msq");

	lb_free();
	return 0;
}

char *pl_alloc_buf(int len) {
	char *optr;

	optr = malloc(len);
	if (!optr) {
		st_err("fail to create buffer");
	}

	return optr;
}

void pl_free_buf(pl_state *ps) {
	if (ps)
		free(ps->buffer);
}

void pl_open_file(char *filename) {
	FILE *src = fopen(filename, "r");
	pl_state ps;

	if (!src) {
		fclose(src);

		st_err("file open error");
	}

	ps.c = 0;
	ps.is_str = 0;
	ps.i = 0;
	ps.buffer = pl_alloc_buf(256);
	
	pl_pars_stream(src, &ps);

	fclose(src);
	pl_free_buf(&ps);
}

void pl_pars_stream(FILE *stream, pl_state *ps) {
	int c = 0;

	c = fgetc(stream);
	while (!feof(stream)) {
		ps->c = c;
		pl_pars_char(ps);

		c = fgetc(stream);
	}
}

void pl_pars_char(pl_state *ps) {
	pl_check_str(ps);
}

void pl_check_str(pl_state *ps) {
	if (ps->c == '\"')
		ps->is_str = !ps->is_str;
	else if (ps->is_str)
		ps->buffer[ps->i++] = ps->c;
	else
		pl_check_space(ps);
}

void pl_check_space(pl_state *ps) {
	if (isspace(ps->c)) {
		ps->buffer[ps->i++] = 0;

		pl_add_token(ps);
		ps->c = 0;
	}
	else
		pl_check_delimetrs(ps);
}

void pl_check_delimetrs(pl_state *ps) {
	if (ps->c == ';') {
		ps->buffer[ps->i++] = 0;

		pl_add_token(ps);
		lb_show_list();

		lb_crawl(il_eval);
		lb_free();
		lb_init();

		ps->c = 0;
	}
	else if (strchr("[]", ps->c)) {
		ps->buffer[ps->i++] = 0;

		pl_add_token(ps);

		ps->buffer[ps->i++] = ps->c;
		ps->buffer[ps->i] = 0;

		pl_add_token(ps);
	}
	else
		pl_check_symbol(ps);
}

void pl_check_symbol(pl_state *ps) {
	if (isalnum(ps->c) || ispunct(ps->c))
		ps->buffer[ps->i++] = ps->c;
	else {
		pl_free_buf(ps);

		st_err("unknown symbol");
	}
}

void pl_show_token(pl_state *ps) {
	if (strlen(ps->buffer))
		printf("Tok: %s\n", ps->buffer);
}

void pl_add_token(pl_state *ps) {
	if (strlen(ps->buffer))
		lb_add_token(ps);

	ps->i = 0;
}
