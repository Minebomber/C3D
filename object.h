#pragma once

#include "mesh.h"
#include "engine.h"

typedef struct {
	float x0, y0, z0;
	float x1, y1, z1;
} box;

typedef struct object {
	mesh mesh;
	vec4 position;
	vec4 velocity;
	vec4 acceleration;
	bool fixed;
	box boundingBox;
	float elasticity;
	float mass;
	void (*cbSetup)(struct object*, engine*);
	void (*cbUpdate)(struct object*, engine*, float);
	void (*cbTeardown)(struct object*, engine*);
	void (*cbCollision)(struct object*, struct object*, float, float, float);
} object;

void object_update(object* o, engine* e, float dt);
box bbox_for_mesh(mesh* m);
bool objects_colliding(object* o1, object* o2);
vec4 object_collision_distance(object* o1, object* o2);

float absmin(float a, float b) {
	return (fabsf(a) < fabsf(b)) ? a : b;
}

vec4 object_collision_distance(object* o1, object* o2) {
	box* b1 = &o1->boundingBox;
	box* b2 = &o2->boundingBox;
	float dx0 = o1->position.x + b1->x0 - (o2->position.x + b2->x1);
	float dy0 = o1->position.y + b1->y0 - (o2->position.y + b2->y1);
	float dz0 = o1->position.z + b1->z0 - (o2->position.z + b2->z1);
	float dx1 = o1->position.x + b1->x1 - (o2->position.x + b2->x0);
	float dy1 = o1->position.y + b1->y1 - (o2->position.y + b2->y0);
	float dz1 = o1->position.z + b1->z1 - (o2->position.z + b2->z0);

	float dx = absmin(dx0, dx1);
	float dy = absmin(dy0, dy1);
	float dz = absmin(dz0, dz1);

	return (vec4) { dx, dy, dz };
}


bool objects_colliding(object* o1, object* o2) {
	box* b1 = &o1->boundingBox; box* b2 = &o2->boundingBox;
	return
		((o1->position.x + b1->x0 <= o2->position.x + b2->x1) && (o1->position.x + b1->x1 >= o2->position.x + b2->x0)) &&
		((o1->position.y + b1->y0 <= o2->position.y + b2->y1) && (o1->position.y + b1->y1 >= o2->position.y + b2->y0)) &&
		((o1->position.z + b1->z0 <= o2->position.z + b2->z1) && (o1->position.z + b1->z1 >= o2->position.z + b2->z0));
}

box bbox_for_mesh(mesh* m) {
	float minX = 0.0f;
	float maxX = 0.0f;
	float minY = 0.0f;
	float maxY = 0.0f;
	float minZ = 0.0f;
	float maxZ = 0.0f;
	bool first = true;
	for (size_t i = 0; i < m->triangles.length; i++) {
		triangle* t = (triangle*)vector_get(&m->triangles, i);
		if (first) {
			minX = t->data[0].x; maxX = t->data[0].x;
			minY = t->data[0].y; maxY = t->data[0].y;
			minZ = t->data[0].z; maxZ = t->data[0].z;
			first = false;
		}

		for (size_t j = 0; j < 3; j++) {
			if (t->data[j].x < minX) minX = t->data[j].x;
			if (t->data[j].x > maxX) maxX = t->data[j].x;
			if (t->data[j].y < minY) minY = t->data[j].y;
			if (t->data[j].y > maxY) maxY = t->data[j].y;
			if (t->data[j].z < minZ) minZ = t->data[j].z;
			if (t->data[j].z > maxZ) maxZ = t->data[j].z;
		}
	}
	return (box) {
		minX, minY, minZ,
		maxX, maxY, maxZ
	};
}

void object_update(object* o, engine* e, float dt) {
	if (!o->fixed) {
		o->velocity = vector_add(o->velocity, vector_mul(o->acceleration, dt));
		if (o->acceleration.x == 0.0f && fabsf(o->velocity.x < 0.1f)) o->velocity.x = 0.0f;
		if (o->acceleration.y == 0.0f && fabsf(o->velocity.y < 0.1f)) o->velocity.y = 0.0f;
		if (o->acceleration.z == 0.0f && fabsf(o->velocity.z < 0.1f)) o->velocity.z = 0.0f;
		o->position = vector_add(o->position, vector_mul(o->velocity, dt));
	}
	o->mesh.matrix = matrix_translation(o->position.x, o->position.y, o->position.z);
}