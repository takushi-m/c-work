#include<ctype.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

enum {
	TK_NUM = 256,
	TK_EOF
};

typedef struct {
	int ty;
	int val;
	char *input;
} Token;

Token tokens[100];

void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void tokenize(char *p) {
	int i = 0;
	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p=='+' || *p=='-') {
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if (isdigit(*p)) {
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}

		error("トークナイズできません: %s", p);
	}

	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}

enum {
	ND_NUM = 256
};

typedef struct Node {
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs) {
	Node *node = malloc(sizeof(Node));
	node->ty = ty;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node = malloc(sizeof(Node));
	node->ty = ND_NUM;
	node->val = val;
	return node;
}

int pos = 0;
int consume(int ty) {
	if (tokens[pos].ty != ty) {
		return 0;
	}
	pos++;
	return 1;
}

Node *add() {
	Node *node;
	if (tokens[pos].ty == TK_NUM) {
		node = new_node_num(tokens[pos++].val);
	} else {
		error("数値でないトークンです: %s", tokens[pos].input);
	}

	for (;;) {
		if (consume('+')) {
			node = new_node('+', node, add());
		} else if (consume('-')) {
			node = new_node('-', node, add());
		} else {
			return node;
		}
	}
}

void gen(Node *node) {
	if (node->ty==ND_NUM) {
		printf("	push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rdx\n");

	switch (node->ty) {
	case '+':
		printf("	add rdx,rdi\n");
		break;
	case '-':
		printf("	sub rdx,rdi\n");
		break;
	default:
		error("未定義のノードです");
		break;
	}

	printf("	push rdx\n");
}

int main(int argc, char **argv) {
	if (argc!=2) {
		error("引数の数が正しくありません");
	}

	tokenize(argv[1]);
	Node *node = add();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
