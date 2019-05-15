// Vector
typedef struct {
	void **data;
	int capacity;
	int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

// Map
typedef struct {
	Vector *keys;
	Vector *vals;
} Map;

Map *new_map();
void map_put(Map *m, char *key, void *val);
void *map_get(Map *m, char *key);


// Token
enum {
	TK_NUM = 256,
	TK_IDENT,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
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
	char *name;
} Token;

Token *add_token(Vector *vec, int ty, char *input);

// Node
enum {
	ND_NUM = 256,
	ND_IDENT,
	ND_RETURN,
	ND_IF,
	ND_ELSE,
	ND_WHILE,
	ND_FOR,
	ND_BLOCK
};

typedef struct Node {
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;  // ty==ND_NUM
	char *name;  // ty==ND_IDENT
	Vector *stmts; // ty==ND_BLOCK
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(char *name);

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
extern int numident;
extern Map *ident;

// codegen
void gen(Node *node);

// util
void error(char *fmt, ...);
int consume(int ty);

// test
void runtest();