#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mcc.h"

Vector *new_vector() {
	Vector *vec = malloc(sizeof(Vector));
	vec->data = malloc(sizeof(void *)*16);
	vec->capacity = 16;
	vec->len = 0;
	return vec;
}

void vec_push(Vector *vec, void *elem) {
	if (vec->capacity==vec->len) {
		vec->capacity *= 2;
		vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
	}
	vec->data[vec->len++] = elem;
}

Map *new_map() {
	Map *m = malloc(sizeof(Map));
	m->keys = new_vector();
	m->vals = new_vector();
	return m;
}

void map_put(Map *m, char *key, void *val) {
	vec_push(m->keys, key);
	vec_push(m->vals, val);
}

void *map_get(Map *m, char *key) {
	for (int i=m->keys->len-1;i>=0;i--) {
		if (strcmp(m->keys->data[i], key)==0) {
			return m->vals->data[i];
		}
	}
	return NULL;
}

void expect(int line, int expected, int actual) {
	if (expected==actual) {
		return;
	} else {
		fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
		exit(1);
	}
}

void test_vector() {
	Vector *vec = new_vector();
	expect(__LINE__, 0, vec->len);

	for (int i=0;i<100;i++) {
		vec_push(vec, (void *)i);
	}

	expect(__LINE__, 100, vec->len);
	expect(__LINE__, 0, (long)vec->data[0]);
	expect(__LINE__, 50, (long)vec->data[50]);
	expect(__LINE__, 99, (long)vec->data[99]);
}

void test_map() {
	Map *map = new_map();
	expect(__LINE__, 0, map->keys->len);
	map_put(map, "abc", (void *)100);
	expect(__LINE__, 100, (long)map_get(map, "abc"));
	map_put(map, "abc", (void *)200);
	expect(__LINE__, 200, (long)map_get(map, "abc"));
}

void runtest() {
	test_vector();
	test_map();

	printf("OK\n");
}