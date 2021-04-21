#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct {
	size_t width, height;
	unsigned short* data;
} texture;

texture texture_create_from_file(const char* path);
texture texture_create_from_color(unsigned short c);
unsigned short texture_get(texture* t, size_t u, size_t v);
void texture_destroy(texture* t);