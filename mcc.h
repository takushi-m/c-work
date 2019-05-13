// Vector
typedef struct {
	void **data;
	int capacity;
	int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);


// Token
enum {
	TK_NUM = 256,
	TK_IDENT,
	TK_RETURN,
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

Token *add_token(Vector *vec, int ty, char *input);

// Node
enum {
	ND_NUM = 256,
	ND_IDENT,
	ND_RETURN
};

typedef struct Node {
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;  // ty==ND_NUM
	char name;  // ty==ND_IDENT
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(char name);

// parse
Vector *tokenize(char *p);
Node *term();
Node *add();
Node *mul();
Node *unary();
Node *equality();
Node *relational();
Node *assign();
Node *expr();
Node *stmt();
Vector *program();

extern Vector *tokens;
extern int pos;
extern Vector *codes;

// codegen
void gen(Node *node);

// util
void error(char *fmt, ...);
int consume(int ty);
