#include<stdio.h>
#include "mcc.h"

int LABEL = 0;

void gen_lval(Node *node) {
	if (node->ty!=ND_IDENT) {
		error("代入の左辺値が変数でありません\n");
	}

	void *os = map_get(ident, node->name);
	if (os==NULL) {
		error("存在しない変数です: %s\n", node->name);
	}
	int offset = (long)os * 8;
	printf("	mov rax,rbp\n");
	printf("	sub rax,%d\n", offset);
	printf("	push rax\n");
}

void gen(Node *node) {
	if (node->ty==ND_NUM) {
		printf("	push %d\n", node->val);
		return;
	}

	if (node->ty==ND_IDENT) {
		gen_lval(node);
		printf("	pop rax\n");
		printf("	mov rax,[rax]\n");
		printf("	push rax\n");
		return;
	}

	if (node->ty==ND_RETURN) {
		gen(node->lhs);
		printf("	pop rax\n");
		printf("	mov rsp,rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
		return;
	}

	if (node->ty==ND_IF) {
		if (node->rhs->ty!=ND_ELSE) {
			// if (A) B
			int label = LABEL++;
			gen(node->lhs); // A
			printf("	pop rax\n");
			printf("	cmp rax,0\n");
			printf("	je .LABEL%d\n", label);
			gen(node->rhs); // B
			printf(".LABEL%d:\n", label);
			return;
		} else {
			// if (A) B else C
			int endlabel = LABEL++;
			int elselabel = LABEL++;
			gen(node->lhs); // A
			printf("	pop rax\n");
			printf("	cmp rax,0\n");
			printf("	je .Lelse%d\n", elselabel);
			gen(node->rhs->lhs); // B
			printf("	jmp .Lend%d\n", endlabel);
			printf(".Lelse%d:\n", elselabel);
			gen(node->rhs->rhs); // C
			printf(".Lend%d:\n", endlabel);
			return;
		}
	}

	if (node->ty==ND_WHILE) {
		int lend = LABEL++;
		int lcond = LABEL++;
		// while (A) B
		printf(".Lcond%d:\n", lcond);
		gen(node->lhs); // A
		printf("	pop rax\n");
		printf("	cmp rax,0\n");
		printf("	je .Lend%d\n", lend);
		gen(node->rhs); // B
		printf("	jmp .Lcond%d\n", lcond);
		printf(".Lend%d:\n", lend);
		return;
	}

	if (node->ty==ND_FOR) {
		int lend = LABEL++;
		int lcond = LABEL++;
		// for (A;B;C) D
		gen(node->lhs); // A
		printf(".Lcond%d:\n", lcond);
		gen(node->rhs->lhs); // B
		printf("	pop rax\n");
		printf("	cmp rax,0\n");
		printf("	je .Lend%d\n", lend);
		gen(node->rhs->rhs->rhs); // D
		gen(node->rhs->rhs->lhs); // C
		printf("	jmp .Lcond%d\n",lcond);
		printf(".Lend%d:\n", lend);
		return;
	}

	if (node->ty=='=') {
		gen_lval(node->lhs);
		gen(node->rhs);

		printf("	pop rdi\n");
		printf("	pop rax\n");
		printf("	mov [rax],rdi\n");
		printf("	push rdi\n");
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