#ifndef interp
#define interp

void il_eval(list *node);
void il_eval_as_func(char *tok, list *node);

void il_run(int code, list *node);

int il_is_block(char *tok);
int il_is_std_func(char *tok);

void il_cb_print(list *node);
void il_cb_help(list *node);
void il_cb_reval(list *node);

#endif
