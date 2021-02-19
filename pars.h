#ifndef pars_h
#define pars_h

typedef struct {
	int c;
	int is_str;

	char *buffer;
	int i;
} pl_state;

char *pl_alloc_buf(int len);
void pl_free_buf(pl_state *ps);
void pl_open_file(char *filename);
void pl_pars_stream(FILE *stream, pl_state *ps);
void pl_pars_char(pl_state *ps);
void pl_check_str(pl_state *ps);
void pl_check_space(pl_state *ps);
void pl_check_symbol(pl_state *ps);

#endif
