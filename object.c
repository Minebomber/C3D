#include "object.h"

void object_update(object* o, engine* e, float dt) {
	if (!o->fixed) {
		o->velocity = vec3_add(o->velocity, vec3_mul_scalar(o->acceleration, dt));
		if (o->acceleration.x == 0.0f && fabsf(o->velocity.x) < 0.1f) o->velocity.x = 0.0f;
		if (o->acceleration.y == 0.0f && fabsf(o->velocity.y) < 0.1f) o->velocity.y = 0.0f;
		if (o->acceleration.z == 0.0f && fabsf(o->velocity.z) < 0.1f) o->velocity.z = 0.0f;
		o->position = vec3_add(o->position, vec3_mul_scalar(o->velocity, dt));
	}
	object_update_matrix(o);
}

void object_update_matrix(object* o) {
	o->mesh.matrix = mat4_scale(o->scale);
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_rotation_z(o->rotation.z));
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_rotation_y(o->rotation.y));
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_rotation_x(o->rotation.x));
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_translation(o->position));
}

void object_collide(object* o1, object* o2, engine* e, float dx, float dy, float dz) {
	if (less(dx, dy) && less(dx, dz)) {
		o1->position.x -= dx;
		o1->velocity.x *= -1.0f * o1->elasticity;
		o1->acceleration.x = 0.0f;
	} else if (less(dy, dx) && less(dy, dz)) {
		o1->position.y -= dy;
		o1->velocity.y *= -1.0f * o1->elasticity;
		o1->acceleration.y = 0.0f;
	} else if (less(dz, dx) && less(dz, dy)) {
		o1->position.z -= dz;
		o1->velocity.z *= -1.0f * o1->elasticity;
		o1->acceleration.z = 0.0f;
	}
	return;
}

bool less(float a, float b) {
	return fabsf(a) < fabsf(b);
}

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

box box_for_mesh(mesh* m) {
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

bbox bbox_for_mesh(mesh* m) {
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
	return (bbox) {
		.data = {
			(vec4) { minX, minY, minZ, 1.0f },
			(vec4) { maxX, minY, minZ, 1.0f },
			(vec4) { maxX, minY, maxZ, 1.0f },
			(vec4) { minX, minY, maxZ, 1.0f },
			(vec4) { minX, maxY, minZ, 1.0f },
			(vec4) { maxX, maxY, minZ, 1.0f },
			(vec4) { maxX, maxY, maxZ, 1.0f },
			(vec4) { minX, maxY, minZ, 1.0f },
		}
	};
}

bbox true_bbox(object* o) {
	return (bbox) {
		.data = {
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[0].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[1].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[2].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[3].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[4].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[5].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[6].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->bBox.data[7].v4) },
		}
	};
}