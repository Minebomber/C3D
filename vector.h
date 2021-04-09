#pragma once

#include <stdlib.h>
#include <string.h>

#define VECTOR_INIT_CAPACITY 4

typedef struct vector {
	size_t capacity;
	size_t itemSize;
	size_t length;
	void* data;
} vector;

vector vector_create(size_t itemSize);
void vector_resize(vector* v, size_t capacity);
void vector_destroy(vector* v);
void* vector_get(vector* v, size_t index);
void vector_append(vector* v, void* element);
void vector_remove(vector* v, size_t index);