all:
	gcc -Wall -O0 -g -o pars pars.c list_builder.c interp.c tl2/list.c
