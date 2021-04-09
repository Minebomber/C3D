#include "linked_list.h"

list_node* list_node_create(size_t itemSize, void* data) {
	list_node* n = malloc(offsetof(list_node, data) + itemSize);
	n->next = NULL;
	n->itemSize = itemSize;
	n->data = NULL;
	memcpy(&n->data, data, itemSize);
	return n;
}

void list_node_push_front(linked_list* l, size_t itemSize, void* data) {
	list_node* n = list_node_create(itemSize, data);
	if (!n) return;
	if (!l->head) {
		l->head = n;
		l->tail = n;
		l->length = 1;
	} else {
		n->next = l->head;
		l->head = n;
		l->length++;
	}
}

void list_push_back(linked_list* l, size_t itemSize, void* data) {
	list_node* n = list_node_create(itemSize, data);
	if (!n) return;
	if (!l->head) {
		l->head = n;
		l->tail = n;
		l->length = 1;
	} else {
		l->tail->next = n;
		l->tail = l->tail->next;
		l->length++;
	}
}

list_node* list_pop_front(linked_list* l) {
	list_node* n = l->head;
	if (!n) return NULL;
	l->head = l->head->next;
	if (!l->head)
		l->tail = NULL;
	l->length--;
	return n;
}

void list_destroy(linked_list* l) {
	while (l->head) {
		list_node* n = l->head;
		l->head = l->head->next;
		free(n);
		l->length--;
	}
	if (l->length != 0) {
		printf("List error: Length != item count\n");
	}
}