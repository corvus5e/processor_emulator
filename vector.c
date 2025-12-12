/*
 *  Generic, debuggable vector in C
 *  Heavily inspired by: https://github.com/glouw/ctl
 *
 *  Discussion on generic data structures in C:
 *  https://news.ycombinator.com/item?id=26735655
*/

#include <stdlib.h>

#ifndef T
#error "T should be defined to use vector"
#endif

#include "vector.h"

#define _JOIN(A, B) A ## B
#define JOIN(A, B) _JOIN(A, B)

#define vec(T) JOIN(vec_, T)

T* JOIN(vec_at_, T)(struct vec(T) * v, size_t index) {
	return &v->data[index];
}


void JOIN(vec_reserve_, T)(struct vec(T) * v, int size) {
	if (size != 0 && v->capacity < size) {
		v->data = (T *)realloc(v->data, sizeof(T) * size);
		v->capacity = size;
	}
}

void JOIN(vec_init_, T)(struct vec(T) * v) {
	v->size = v->capacity = 0;
	v->data = NULL;
}

void JOIN(vec_free_, T)(struct vec(T) * v) {
	free(v->data);
	v->size = v->capacity = 0;
}


void JOIN(vec_push_back_, T)(struct vec(T) * v, T value) {
	if (v->size == v->capacity) {
		JOIN(vec_reserve_, T)(v, v->capacity == 0 ? 1 : v->capacity * 2);
	}
	*JOIN(vec_at_, T)(v, v->size) = value;
	v->size += 1;
}


