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

#ifndef _VECTOR__H__
#define _VECTOR__H__

#define _JOIN(A, B) A ## B
#define JOIN(A, B) _JOIN(A, B)

#define vec(T) JOIN(vec_, T)
#endif

struct vec(T) {
	T *data;
	size_t size;
	size_t capacity;
};


T* JOIN(vec_at_, T)(struct vec(T) * v, size_t index);

void JOIN(vec_reserve_, T)(struct vec(T) * v, int size);

void JOIN(vec_init_, T)(struct vec(T) * v);

void JOIN(vec_free_, T)(struct vec(T) * v);

void JOIN(vec_push_back_, T)(struct vec(T) * v, T value);

