#include "st.h"

st_name("sample.txt");
st_debug_start(1);

void f1() {
	st_step("in f1");
	st_log("in f1 function\n");
}

void f2() {
	st_logf("in f2");

	st_debug {
		st_log("This will be written only in debug mode\n");
		st_err("This is error");
	}
}

int main() {
	st_start();
	st_descr("This is sample usage of st.h");

	st_logf("f1 call");
	st_step("call f1");
	f1();

	st_logf("f2 call");
	st_step("call f2");
	f2();

	st_end();
	return 0;
}
