#pragma once
#include <math.h>

typedef struct {
	float x, y, z, w;
} vec4;

typedef struct {
	float data[4][4];
} mat4;

vec4	vector_add(vec4 u, vec4 v);
vec4	vector_sub(vec4 u, vec4 v);
vec4	vector_mul(vec4 v, float f);
vec4	vector_div(vec4 v, float f);

float	vector_length(vec4 v);
vec4	vector_normalize(vec4 v);
vec4	vector_scale_w(vec4 v);

float	vector_dot(vec4 u, vec4 v);
vec4	vector_cross(vec4 u, vec4 v);

vec4	matrix_multiply_vector(mat4 m, vec4 v);
mat4	matrix_multiply_matrix(mat4 m, mat4 n);

mat4	matrix_identity();
mat4	matrix_translation(float x, float y, float z);
mat4	matrix_rotation_x(float angle);
mat4	matrix_rotation_y(float angle);
mat4	matrix_rotation_z(float angle);
mat4	matrix_projection(float fov, float aspect, float zNear, float zFar);
mat4	matrix_point_at(vec4 pos, vec4 target, vec4 up);
mat4	matrix_quick_inverse(mat4 m);

vec4	vector_intersect_plane(vec4 planeP, vec4 planeN, vec4 lineStart, vec4 lineEnd);
float	vector_dist_to_plane(vec4 planeP, vec4 planeN, vec4 v);

typedef struct {
	float x, y;
} vec2;
