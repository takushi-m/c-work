#include<ctype.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

enum {
	TK_NUM = 256,
	TK_EOF,
	TK_EQ,
	TK_NE,
	TK_LE,
	TK_GE,
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

		if (strncmp(p,"==",2)==0) {
			tokens[i].ty = TK_EQ;
			tokens[i].input = p;
			i++;
			p += 2;
			continue;
		}
		if (strncmp(p,"!=",2)==0) {
			tokens[i].ty = TK_NE;
			tokens[i].input = p;
			i++;
			p += 2;
			continue;
		}
		if (strncmp(p,"<=",2)==0) {
			tokens[i].ty = TK_LE;
			tokens[i].input = p;
			i++;
			p += 2;
			continue;
		}
		if (strncmp(p,">=",2)==0) {
			tokens[i].ty = TK_GE;
			tokens[i].input = p;
			i++;
			p += 2;
			continue;
		}

		if (*p=='+' || *p=='-' || *p=='*' || *p=='/' || *p=='(' || *p==')' || *p=='<' || *p=='>') {
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

Node *term();
Node *add();
Node *mul();
Node *unary();
Node *equality();
Node *relational();

Node *equality() {
	Node *node = relational();

	for (;;) {
		if (consume(TK_EQ)) {
			node = new_node(TK_EQ, node, relational());
		} else if (consume(TK_NE)) {
			node = new_node(TK_NE, node, relational());
		} else {
			return node;
		}
	}
}

Node *relational() {
	Node *node = add();

	for (;;) {
		if (consume(TK_LE)) {
			node = new_node(TK_LE, node, add());
		} else if (consume('<')) {
			node = new_node('<', node, add());
		} else if (consume(TK_GE)) {
			node = new_node(TK_LE, add(), node);
		} else if (consume('>')) {
			node = new_node('<', add(), node);
		} else {
			return node;
		}
	}
}

Node *unary() {
	if (consume('+')) {
		return term();
	} else if (consume('-')) {
		return new_node('-', new_node_num(0), term());
	} else {
		return term();
	}
}

Node *term() {
	if (consume('(')) {
		Node *node = add();
		if (!consume(')')) {
			error("閉じ括弧がありません: %s", tokens[pos].input);
		}
		return node;
	}

	if (tokens[pos].ty==TK_NUM) {
		Node *node = new_node_num(tokens[pos++].val);
		return node;
	}

	error("定義されていないトークンです: %s", tokens[pos].input);
}

Node *mul() {
	Node *node = unary();

	for (;;) {
		if (consume('*')) {
			node = new_node('*', node, unary());
		} else if (consume('/')) {
			node = new_node('/', node, unary());
		} else {
			return node;
		}
	}
}

Node *add() {
	Node *node = mul();

	for (;;) {
		if (consume('+')) {
			node = new_node('+', node, mul());
		} else if (consume('-')) {
			node = new_node('-', node, mul());
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
	printf("	pop rax\n");

	switch (node->ty) {
	case '+':
		printf("	add rax,rdi\n");
		break;
	case '-':
		printf("	sub rax,rdi\n");
		break;
	case '*':
		printf("	mul rdi\n");
		break;
	case '/':
		printf("	mov rdx,0\n");
		printf("	div rdi\n");
		break;
	case TK_EQ:
		printf("	cmp rax,rdi\n");
		printf("	sete al\n");
		printf("	movzb rax,al\n");
		break;
	case TK_NE:
		printf("	cmp rax,rdi\n");
		printf("	setne al\n");
		printf("	movzb rax,al\n");
		break;
	case TK_LE:
		printf("	cmp rax,rdi\n");
		printf("	setle al\n");
		printf("	movzb rax,al\n");
		break;
	case '<':
		printf("	cmp rax,rdi\n");
		printf("	setl al\n");
		printf("	movzb rax,al\n");
		break;
	default:
		error("未定義のノードです");
		break;
	}

	printf("	push rax\n");
}

int main(int argc, char **argv) {
	if (argc!=2) {
		error("引数の数が正しくありません");
	}

	tokenize(argv[1]);
	Node *node = equality();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
