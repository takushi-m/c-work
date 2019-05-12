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
	ND_NUM = 256
};

typedef struct Node {
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);

// parse
Vector *tokenize(char *p);
Node *term();
Node *add();
Node *mul();
Node *unary();
Node *equality();
Node *relational();

extern Vector *tokens;
extern int pos;

// codegen
void gen(Node *node);

// util
void error(char *fmt, ...);
int consume(int ty);
