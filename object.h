#pragma once

#include "mesh.h"
#include "engine.h"

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
} bbox;

typedef struct object {
	mesh mesh;
	vec3 position;
	vec3 velocity;
	vec3 acceleration;
	bool fixed;
	box boundingBox;
	bbox bBox;
	float elasticity;
	vec3 scale;
	vec3 rotation;
	void (*cbSetup)(struct object*, engine*);
	void (*cbUpdate)(struct object*, engine*, float);
	void (*cbTeardown)(struct object*, engine*);
	void (*cbCollision)(struct object*, struct object*, engine*, float, float, float);
} object;

void object_update_matrix(object* o);
void object_update(object* o, engine* e, float dt);
void object_collide(object* o1, object* o2, engine* e, float dx, float dy, float dz);
box box_for_mesh(mesh* m);
bool objects_colliding(object* o1, object* o2);
vec4 object_collision_distance(object* o1, object* o2);
float absmin(float a, float b);
bool less(float a, float b);
bbox bbox_for_mesh(mesh* m);
bbox true_bbox(object* o);
