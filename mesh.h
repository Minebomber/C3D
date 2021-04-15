#pragma once
#pragma warning(disable:4996)

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "vmath2.h"
#include "vector.h"
#include "constants.h"

typedef struct {
	vec4u data[3];
	wchar_t symbol;
	unsigned short color;
} triangle;

typedef struct {
	mat4 matrix;
	vector triangles;
	float r, g, b;
} mesh;

int triangle_clip(vec3 planeP, vec3 planeN, triangle* toClip, triangle* clipped1, triangle* clipped2);
int triangle_compare(const void* a, const void* b);
triangle triangle_multiply_matrix(triangle t, mat4* m, bool scale);
mesh mesh_from_obj(const char* path);
