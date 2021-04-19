#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "vmath2.h"
#include "vector.h"
#include "constants.h"

typedef struct engine engine;

typedef struct {
	vec4u data[3];
	wchar_t symbol;
	unsigned short color;
} triangle;



typedef struct {
	float x0, y0, z0;
	float x1, y1, z1;
} box;

/*
  7-6
 4-5

  3-2    y z
 0-1     * x
*/
typedef struct {
	vec4u data[8];
	vec4u xAxis, yAxis, zAxis;
} bbox;

typedef struct object {
	mat4 matrix;
	vector triangles;
	vec3 position;
	vec3 velocity;
	vec3 acceleration;
	bool fixed;
	bbox boundingBox;
	float elasticity;
	vec3 scale;
	vec3 rotation;
	unsigned short color;
	float mass;
	void (*cbSetup)(struct object*, engine*);
	void (*cbUpdate)(struct object*, engine*, float);
	void (*cbTeardown)(struct object*, engine*);
	void (*cbCollision)(struct object*, struct object*, engine*, vec3);
} object;

int triangle_clip(vec3 planeP, vec3 planeN, triangle* toClip, triangle* clipped1, triangle* clipped2);
int triangle_compare(const void* a, const void* b);
triangle triangle_multiply_matrix(triangle t, mat4* m, bool scale);

object object_create_from_obj(const char* path);
void object_update_matrix(object* o);
void object_update(object* o, engine* e, float dt);
void object_collide(object* o1, object* o2, engine* e, vec3 col);
void object_setup(object* o, engine* e);
void object_teardown(object* o, engine* e);

float absmin(float a, float b);
bool less(float a, float b);

bbox bbox_for_triangles(vector* v);
bbox true_bbox(object* o);
vector triangles_from_obj(const char* path);

void bbox_project(vec3 a, bbox* b, float* mn, float* mx);
bool does_overlap(float mn1, float mx1, float mn2, float mx2);
bool sat_test_axis(vec3 a, bbox* b1, bbox* b2, float* mnOv, vec3* mnOvRes);
bool valid_axis(vec3 a, float e);
bool objects_colliding_sat(object* o1, object* o2, vec3* overlap);