#include "object.h"
#pragma warning(disable:4996)

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
	o->matrix = mat4_scale(o->scale);
	o->matrix = mat4_mul_mat4(&o->matrix, mat4_rotation(o->rotation));
	o->matrix = mat4_mul_mat4(&o->matrix, mat4_translation(o->position));
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

int triangle_clip(vec3 planeP, vec3 planeN, triangle* toClip, triangle* clipped1, triangle* clipped2) {
	planeN = vec3_normalize(planeN);

	vec4u* insidePoints[3]; size_t insideCount = 0;
	vec4u* outsidePoints[3]; size_t outsideCount = 0;

	float d0 = vec3_dist_to_plane(planeP, planeN, toClip->data[0].v3);
	float d1 = vec3_dist_to_plane(planeP, planeN, toClip->data[1].v3);
	float d2 = vec3_dist_to_plane(planeP, planeN, toClip->data[2].v3);

	if (d0 >= 0.0f) { insidePoints[insideCount++] = &toClip->data[0]; } else { outsidePoints[outsideCount++] = &toClip->data[0]; }

	if (d1 >= 0.0f) { insidePoints[insideCount++] = &toClip->data[1]; } else { outsidePoints[outsideCount++] = &toClip->data[1]; }

	if (d2 >= 0.0f) { insidePoints[insideCount++] = &toClip->data[2]; } else { outsidePoints[outsideCount++] = &toClip->data[2]; }

	if (insideCount == 0) {
		return 0;
	}

	if (insideCount == 3) {
		*clipped1 = *toClip;
		return 1;
	}

	if (insideCount == 1 && outsideCount == 2) {
		clipped1->color = FG_BLUE; // toClip->color;
		clipped1->symbol = toClip->symbol;

		clipped1->data[0] = *insidePoints[0];

		clipped1->data[1] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[0]->v3)}; clipped1->data[1].w = 1.0f;
		clipped1->data[2] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[1]->v3) }; clipped1->data[2].w = 1.0f;

		return 1;
	}

	if (insideCount == 2 && outsideCount == 1) {
		clipped1->color = FG_GREEN; // toClip->color;
		clipped1->symbol = toClip->symbol;
		clipped2->color = FG_RED; // toClip->color;
		clipped2->symbol = toClip->symbol;

		clipped1->data[0] = *insidePoints[0];
		clipped1->data[1] = *insidePoints[1];
		clipped1->data[2] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[0]->v3) }; clipped1->data[2].w = 1.0f;

		clipped2->data[0] = *insidePoints[1];
		clipped2->data[1] = clipped1->data[2];
		clipped2->data[2] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[1]->v3, outsidePoints[0]->v3) }; clipped2->data[2].w = 1.0f;

		return 2;
	}

	return 0;
}

int triangle_compare(const void* a, const void* b) {
	const triangle* t1 = a;
	const triangle* t2 = b;
	float z1 = t1->data[0].z + t1->data[1].z + t1->data[2].z;
	float z2 = t2->data[0].z + t2->data[1].z + t2->data[2].z;
	if (z1 < z2) return 1;
	if (z1 > z2) return -1;
	return 0;
}

triangle triangle_multiply_matrix(triangle t, mat4* m, bool scale) {
	triangle u = {
		(vec4u) { .v4 = mat4_mul_vec4(m, t.data[0].v4) },
		(vec4u) { .v4 = mat4_mul_vec4(m, t.data[1].v4) },
		(vec4u) { .v4 = mat4_mul_vec4(m, t.data[2].v4) },
	};
	if (scale) {
		u.data[0].v4 = vec4_scale_w(u.data[0].v4);
		u.data[1].v4 = vec4_scale_w(u.data[1].v4);
		u.data[2].v4 = vec4_scale_w(u.data[2].v4);
	}
	return u;
}

box box_for_triangles(vector* v) {
	float minX = 0.0f;
	float maxX = 0.0f;
	float minY = 0.0f;
	float maxY = 0.0f;
	float minZ = 0.0f;
	float maxZ = 0.0f;
	bool first = true;
	for (size_t i = 0; i < v->length; i++) {
		triangle* t = (triangle*)vector_get(v, i);
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

bbox bbox_for_triangles(vector* v) {
	float minX = 0.0f;
	float maxX = 0.0f;
	float minY = 0.0f;
	float maxY = 0.0f;
	float minZ = 0.0f;
	float maxZ = 0.0f;
	bool first = true;
	for (size_t i = 0; i < v->length; i++) {
		triangle* t = (triangle*)vector_get(v, i);
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
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[0].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[1].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[2].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[3].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[4].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[5].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[6].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->bBox.data[7].v4) },
		}
	};
}

vector triangles_from_obj(const char* path) {
	FILE* obj = fopen(path, "r");
	vector vertices = vector_create(sizeof(vec4));
	vector triangles = vector_create(sizeof(triangle));
	char line[128];

	while (fgets(line, sizeof(line), obj)) {
		if (line[0] == 'v') {
			char junk;
			float x, y, z;
			int _ = sscanf(line, "%c %f %f %f", &junk, &x, &y, &z);
			vec4 v = { x, y, z, 1.0f };
			vector_append(&vertices, &v);
		}

		if (line[0] == 'f') {
			char junk;
			size_t i, j, k;
			int _ = sscanf(line, "%c %d %d %d", &junk, &i, &j, &k);
			triangle t = {
				{
					(vec4u) { .v4 = *(vec4*)vector_get(&vertices, i - 1) },
					(vec4u) { .v4 = *(vec4*)vector_get(&vertices, j - 1) },
					(vec4u) { .v4 = *(vec4*)vector_get(&vertices, k - 1) },
				},
				0, 0
			};
			vector_append(&triangles, &t);
		}
	}

	vector_destroy(&vertices);
	fclose(obj);

	return triangles;
}