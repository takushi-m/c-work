#include<stdlib.h>

#include "mcc.h"

Vector *tokens;
int pos;

int consume(int ty) {
	Token *t = tokens->data[pos];
	if (t->ty != ty) {
		return 0;
	}
	pos++;
	return 1;
}

Token *add_token(Vector *vec, int ty, char *input) {
	Token *tk = malloc(sizeof(Token));
	tk->ty = ty;
	tk->input = input;

	vec_push(vec, tk);
	return tk;
}

Vector *tokenize(char *p) {
	Vector *v = new_vector();
	int i = 0;
	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (strncmp(p,"==",2)==0) {
			add_token(v, TK_EQ, p);
			i++;
			p += 2;
			continue;
		}
		if (strncmp(p,"!=",2)==0) {
			add_token(v, TK_NE, p);
			i++;
			p += 2;
			continue;
		}
		if (strncmp(p,"<=",2)==0) {
			add_token(v, TK_LE, p);
			i++;
			p += 2;
			continue;
		}
		if (strncmp(p,">=",2)==0) {
			add_token(v, TK_GE, p);
			i++;
			p += 2;
			continue;
		}

		if (*p=='+' || *p=='-' || *p=='*' || *p=='/' || *p=='(' || *p==')' || *p=='<' || *p=='>') {
			add_token(v, *p, p);
			i++;
			p++;
			continue;
		}

		if (isdigit(*p)) {
			Token *tk = add_token(v, TK_NUM, p);
			tk->val = strtol(p, &p, 10);
			i++;
			continue;
		}

		error("トークナイズできません: %s", p);
	}

	add_token(v, TK_EOF, p);
	return v;
}

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
	Token *t = tokens->data[pos];
	if (consume('(')) {
		Node *node = add();
		if (!consume(')')) {
			error("閉じ括弧がありません: %s", t->input);
		}
		return node;
	}

	if (t->ty==TK_NUM) {
		pos++;
		Node *node = new_node_num(t->val);
		return node;
	}

	error("定義されていないトークンです: %s", t->input);
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