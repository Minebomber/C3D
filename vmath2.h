#pragma once

#include <math.h>

#define M_PI		3.14159265358979323846f
#define M_PI_2		1.57079632679489661923f
#define M_PI_4		0.78539816339744830962f
#define M_1_PI		0.31830988618379067154f
#define M_2_PI		0.63661977236758134308f

typedef struct {
	float x, y;
} vec2;

typedef struct {
	float x, y, z;
} vec3;

typedef struct {
	float x, y, z, w;
} vec4;

typedef union {
	vec3;
	vec2 v2;
	vec3 v3;
} vec3u;

typedef union {
	vec4;
	vec2 v2;
	vec3 v3;
	vec4 v4;
} vec4u;

typedef union {
	float data[3][3];
} mat3;

typedef union {
	float data[4][4];
} mat4;

float degrees(float r);
float radians(float d);

vec2 vec2_add(vec2 u, vec2 v);
vec3 vec3_add(vec3 u, vec3 v);
vec4 vec4_add(vec4 u, vec4 v);

vec2 vec2_sub(vec2 u, vec2 v);
vec3 vec3_sub(vec3 u, vec3 v);
vec4 vec4_sub(vec4 u, vec4 v);

vec2 vec2_mul_scalar(vec2 v, float f);
vec3 vec3_mul_scalar(vec3 v, float f);
vec4 vec4_mul_scalar(vec4 v, float f);

vec2 vec2_mul_vec2(vec2 u, vec2 v);
vec3 vec3_mul_vec3(vec3 u, vec3 v);
vec4 vec4_mul_vec4(vec4 u, vec4 v);
				   
vec2 vec2_div_scalar(vec2 v, float f);
vec3 vec3_div_scalar(vec3 v, float f);
vec4 vec4_div_scalar(vec4 v, float f);

vec2 vec2_div_vec2(vec2 u, vec2 v);
vec3 vec3_div_vec3(vec3 u, vec3 v);
vec4 vec4_div_vec4(vec4 u, vec4 v);

float vec2_dot(vec2 u, vec2 v);
float vec3_dot(vec3 u, vec3 v);

vec3 vec3_cross(vec3 u, vec3 v);

float vec2_length(vec2 v);
float vec3_length(vec3 v);

vec2 vec2_normalize(vec2 v);
vec3 vec3_normalize(vec3 v);

vec4 vec4_scale_w(vec4 v);

vec3 vec3_project(vec3 u, vec3 v);

vec3 mat3_mul_vec3(mat3* m, vec3 v);
vec4 mat4_mul_vec4(mat4* m, vec4 v);

mat3 mat3_mul_mat3(mat3* m, mat3 n);
mat4 mat4_mul_mat4(mat4* m, mat4 n);

mat3 mat3_identity();
mat4 mat4_identity();

mat3 mat3_translation(vec2 v);
mat4 mat4_translation(vec3 v);

mat4 mat4_rotation_x(float f);
mat4 mat4_rotation_y(float f);
mat4 mat4_rotation_z(float f);

mat3 mat3_rotation(float f);
mat4 mat4_rotation(vec3 v);

mat3 mat3_scale(vec2 v);
mat4 mat4_scale(vec3 v);

mat4 mat4_projection(float fov, float aspect, float zNear, float zFar);
mat4 mat4_point_at(vec3 pos, vec3 target, vec3 up);
mat4 mat4_quick_inverse(mat4* m);

vec3 vec3_intersect_plane(vec3 planeP, vec3 planeN, vec3 lineStart, vec3 lineEnd, float* tVal);
float vec3_dist_to_plane(vec3 planeP, vec3 planeN, vec3 v);