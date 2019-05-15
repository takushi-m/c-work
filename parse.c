#include<stdlib.h>
#include<ctype.h>
#include<string.h>

#include "mcc.h"

Vector *tokens;
int pos;
Vector *codes;
int numident;
Map * ident;

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

int is_alnum(char c) {
	return  ('a'<=c && c<='z') ||
			('A'<=c && c<='Z') ||
			('0'<=c && c<='9') ||
			c=='_';
}

Vector *tokenize(char *p) {
	Vector *v = new_vector();
	int i = 0;
	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (strncmp(p,"return",6)==0 && !is_alnum(p[6])) {
			add_token(v, TK_RETURN, p);
			i++;
			p += 6;
			continue;
		}

		if (strncmp(p,"while",5)==0) {
			add_token(v, TK_WHILE, p);
			i++;
			p += 5;
			continue;
		}

		if (strncmp(p,"else",4)==0) {
			add_token(v, TK_ELSE, p);
			i++;
			p += 4;
			continue;
		}

		if (strncmp(p,"if",2)==0) {
			add_token(v, TK_IF, p);
			i++;
			p += 2;
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

		if (isalpha(p[0])) {
			int len = 0;
			while (is_alnum(p[++len]));
			Token *tk = add_token(v, TK_IDENT, p);
			tk->name = malloc(sizeof(char)*len);
			strncpy(tk->name, p, len);
			i++;
			p += len;
			continue;
		}

		if (*p=='+' || *p=='-' || *p=='*' || *p=='/' || *p=='(' || *p==')' || *p=='<' || *p=='>' || *p==';' || *p=='=') {
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

Node *add_node(Vector *vec, Node *node) {
	vec_push(vec, node);
	return node;
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

Node *new_node_ident(char *name) {
	Node *node = malloc(sizeof(Node));
	node->ty = ND_IDENT;
	node->name = name;

	void *val = map_get(ident, name);
	if (val==NULL) {
		map_put(ident, name, (void *)++numident);
	}
	return node;
}

Vector *program() {
	Vector *v = new_vector();
	Token *t = tokens->data[pos];
	while (t->ty != TK_EOF) {
		Node *node = stmt();
		add_node(v, node);
		t = tokens->data[pos];
	}
	return v;
}

Node *stmt() {
	Node *n;
	if (consume(TK_IF)) {
		if (!consume('(')) {
			error("ifの条件式がありません");
		}
		n = malloc(sizeof(Node));
		n->ty = ND_IF;
		n->lhs = expr();
		if (!consume(')')) {
			error("ifの条件式が閉じられていません");
		}
		Node *then = stmt();
		if (consume(TK_ELSE)) {
			Node *el = malloc(sizeof(Node));
			el->ty = ND_ELSE;
			el->lhs = then;
			el->rhs = stmt();
			n->rhs = el;
		} else {
			n->rhs = then;
		}
		return n;
	}

	if (consume(TK_WHILE)) {
		if (!consume('(')) {
			error("whileの条件式がありません");
		}
		n = malloc(sizeof(Node));
		n->ty = ND_WHILE;
		n->lhs = expr();
		if (!consume(')')) {
			error("whileの条件式が閉じられていません");
		}
		n->rhs = stmt();
		return n;
	}
	
	if (consume(TK_RETURN)) {
		n = malloc(sizeof(Node));
		n->ty = ND_RETURN;
		n->lhs = expr();
	} else {
		n = expr();
	}

	Token *t = tokens->data[pos];
	if (!consume(';')) {
		error("';'でないトークンです: %s", t->input);
	}
	return n;
}

Node *expr() {
	Node *node = assign();
	return node;
}

Node *assign() {
	Node *n = equality();
	if (consume('=')) {
		n = new_node('=', n, assign());
	}
	return n;
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

	if (t->ty==TK_IDENT) {
		pos++;
		Node *node = new_node_ident(t->name);
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