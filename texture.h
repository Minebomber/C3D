#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct {
	size_t width, height;
	unsigned short* data;
} texture;

texture texture_create_from_file(const char* path);
texture texture_create_from_color(unsigned short c);
unsigned short texture_get_xy(texture* t, size_t x, size_t y);
unsigned short texture_get_uv(texture* t, float u, float v);
void texture_destroy(texture* t);