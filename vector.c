#include "vector.h"

vector vector_create(size_t itemSize) {
	return (vector) {
		.capacity = VECTOR_INIT_CAPACITY,
			.itemSize = itemSize,
			.length = 0,
			.data = malloc(itemSize * VECTOR_INIT_CAPACITY),
	};
}

void vector_resize(vector* v, size_t capacity) {
	void* data = realloc(v->data, v->itemSize * capacity);
	if (data) {
		v->data = data;
		v->capacity = capacity;
	}
}

void vector_destroy(vector* v) {
	free(v->data);
	v->data = NULL;
	v->capacity = -1;
	v->length = -1;
}

void* vector_get(vector* v, size_t index) {
	if (index < 0 || index >= v->length)
		return NULL;
	return (char*)v->data + (index * v->itemSize);
}

void vector_append(vector* v, void* element) {
	if (v->length == v->capacity) vector_resize(v, v->capacity * 2);
	memcpy((char*)v->data + (v->length++ * v->itemSize), element, v->itemSize);
}

void vector_remove(vector* v, size_t index) {
	if (index < 0 || index >= v->length)
		return;
	for (size_t i = index; i < v->length - 1; i++) {
		memcpy(vector_get(v, i), vector_get(v, i + 1), v->itemSize);
	}
	v->length--;
	if (v->length > 0 && v->length == v->capacity / 4)
		vector_resize(v, v->capacity / 2);
}