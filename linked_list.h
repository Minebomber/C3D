#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct list_node {
	struct list_node* next;
	size_t itemSize;
	void* data;
} list_node;

typedef struct {
	list_node* head;
	list_node* tail;
	size_t length;
} linked_list;

list_node* list_node_create(size_t itemSize, void* data);
void list_node_push_front(linked_list* l, size_t itemSize, void* data);
void list_push_back(linked_list* l, size_t itemSize, void* data);
list_node* list_pop_front(linked_list* l);
void list_destroy(linked_list* l);

