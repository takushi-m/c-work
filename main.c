#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include<stdlib.h>

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
	ident = new_map();
	codes = program();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// プロローグ
	printf("	push rbp\n");
	printf("	mov rbp,rsp\n");
	printf("	sub rsp,%d\n", numident*8);

	// 本体
	for (int i=0;i<codes->len;i++) {
		gen(codes->data[i]);
		printf("	pop rax\n");
	}

	// エピローグ
	printf("	mov rsp,rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
	return 0;
}
