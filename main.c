#include<stdio.h>
#include<string.h>
#include<stdarg.h>

#include "mcc.h"

void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

int main(int argc, char **argv) {
	if (argc!=2) {
		error("引数の数が正しくありません");
	}

	if (strncmp(argv[1], "-test", 5)==0) {
		runtest();
		return 0;
	}

	tokens = tokenize(argv[1]);
	Node *node = equality();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
