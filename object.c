#include "object.h"
#pragma warning(disable:4996)

object object_create_from_obj(const char* path) {
	vector t = triangles_from_obj(path);
	object o = {
		.cbSetup = object_setup,
		.cbUpdate = object_update,
		.cbTeardown = object_teardown,
		.cbCollision = object_collide,

		.triangles = t,
		.matrix = mat4_identity(),
		.boundingBox = bbox_for_triangles(&t),

		.position = {0, 0, 0},
		.velocity = {0, 0, 0},
		.acceleration = {0, 0, 0},

		.scale = {1.0f, 1.0f, 1.0f},
		.rotation = {0.0f, 0.0f, 0.0f},

		.color = FG_WHITE,
	};
	return o;
}

void object_update(object* o, engine* e, float dt) {
	o->prevPosition = o->position;
	if (!o->fixed) {
		o->velocity = vec3_add(o->velocity, vec3_mul_scalar(o->acceleration, dt));
		if (o->acceleration.x == 0.0f && fabsf(o->velocity.x) < 0.5f) o->velocity.x = 0.0f;
		if (o->acceleration.y == 0.0f && fabsf(o->velocity.y) < 0.5f) o->velocity.y = 0.0f;
		if (o->acceleration.z == 0.0f && fabsf(o->velocity.z) < 0.5f) o->velocity.z = 0.0f;
		o->position = vec3_add(o->position, vec3_mul_scalar(o->velocity, dt));
	}
	if (fabsf(o->position.x) > 100.0f) o->position.x *= -1.0f;
	if (fabsf(o->position.y) > 100.0f) o->position.y *= -1.0f;
	if (fabsf(o->position.z) > 100.0f) o->position.z *= -1.0f;
	object_update_matrix(o);
}

void object_update_matrix(object* o) {
	o->matrix = mat4_scale(o->scale);
	o->matrix = mat4_mul_mat4(&o->matrix, mat4_rotation(o->rotation));
	o->matrix = mat4_mul_mat4(&o->matrix, mat4_translation(o->position));
}

void object_collide(object* o1, object* o2, engine* e, vec3 col) {
	o1->position = o1->prevPosition;
	vec3 n = vec3_normalize(col);
	vec3 v = vec3_project(o1->velocity, n);
	o1->velocity = vec3_sub(o1->velocity, vec3_mul_scalar(v, 1.0f + o1->elasticity));
	if (o2->fixed) {
		vec3 u = vec3_project(o1->acceleration, n);
		o1->acceleration = vec3_sub(o1->acceleration, u);
	}
}

void object_setup(object* o, engine* e) {
	object_update_matrix(o);
	o->prevPosition = o->position;
}

void object_teardown(object* o, engine* e) {

}

bool less(float a, float b) {
	return fabsf(a) < fabsf(b);
}

float absmin(float a, float b) {
	return (fabsf(a) < fabsf(b)) ? a : b;
}

void bbox_project(vec3 a, bbox* b, float* mn, float* mx) {
	*mn = INFINITY; *mx = -INFINITY;
	for (size_t i = 0; i < 8; i++) {
		float v = vec3_dot(a, b->data[i].v3);
		if (v < *mn) *mn = v;
		if (v > *mx) *mx = v;
	}
}

bool does_overlap(float mn1, float mx1, float mn2, float mx2) {
	float l = max(mx1, mx2) - min(mn1, mn2);
	float s = mx1 - mn1 + mx2 - mn2;
	return l < s;
}

bool sat_test_axis(vec3 a, bbox* b1, bbox* b2, float* mnOv, vec3* mnOvRes) {
	float mn1, mx1, mn2, mx2;
	bbox_project(a, b1, &mn1, &mx1);
	bbox_project(a, b2, &mn2, &mx2);
	if (does_overlap(mn1, mx1, mn2, mx2)) {
		float ov = min(mx1, mx2) - max(mn1, mn2);
		if (ov < *mnOv) {
			*mnOv = ov;
			*mnOvRes = vec3_mul_scalar(a, ov);
		}
		return true;
	}
	return false;
}

bool valid_axis(vec3 a, float e) {
	return fabsf(a.x) > e && fabsf(a.y) > e && fabsf(a.z) > e;
}

bool objects_colliding_sat(object* o1, object* o2, vec3* overlap) {
	bbox t1 = true_bbox(o1);
	bbox t2 = true_bbox(o2);

	float minOverlap = INFINITY;

	vec3 x1x2 = vec3_cross(t2.xAxis.v3, t1.xAxis.v3);
	vec3 x1y2 = vec3_cross(t2.xAxis.v3, t1.yAxis.v3);
	vec3 x1z2 = vec3_cross(t2.xAxis.v3, t1.zAxis.v3);

	vec3 y1x2 = vec3_cross(t2.yAxis.v3, t1.xAxis.v3);
	vec3 y1y2 = vec3_cross(t2.yAxis.v3, t1.yAxis.v3);
	vec3 y1z2 = vec3_cross(t2.yAxis.v3, t1.zAxis.v3);

	vec3 z1x2 = vec3_cross(t2.zAxis.v3, t1.xAxis.v3);
	vec3 z1y2 = vec3_cross(t2.zAxis.v3, t1.yAxis.v3);
	vec3 z1z2 = vec3_cross(t2.zAxis.v3, t1.zAxis.v3);

	return (
		sat_test_axis(t1.xAxis.v3, &t1, &t2, &minOverlap, overlap) &&
		sat_test_axis(t1.yAxis.v3, &t1, &t2, &minOverlap, overlap) &&
		sat_test_axis(t1.zAxis.v3, &t1, &t2, &minOverlap, overlap) &&
		sat_test_axis(t2.xAxis.v3, &t1, &t2, &minOverlap, overlap) &&
		sat_test_axis(t2.yAxis.v3, &t1, &t2, &minOverlap, overlap) &&
		sat_test_axis(t2.zAxis.v3, &t1, &t2, &minOverlap, overlap) &&

		(valid_axis(x1x2, 0.1f) ? sat_test_axis(x1x2, &t1, &t2, &minOverlap, overlap) : true) &&
		(valid_axis(x1y2, 0.1f) ? sat_test_axis(x1y2, &t1, &t2, &minOverlap, overlap) : true) &&
		(valid_axis(x1z2, 0.1f) ? sat_test_axis(x1z2, &t1, &t2, &minOverlap, overlap) : true) &&

		(valid_axis(y1x2, 0.1f) ? sat_test_axis(y1x2, &t1, &t2, &minOverlap, overlap) : true) &&
		(valid_axis(y1y2, 0.1f) ? sat_test_axis(y1y2, &t1, &t2, &minOverlap, overlap) : true) &&
		(valid_axis(y1z2, 0.1f) ? sat_test_axis(y1z2, &t1, &t2, &minOverlap, overlap) : true) &&

		(valid_axis(z1x2, 0.1f) ? sat_test_axis(z1x2, &t1, &t2, &minOverlap, overlap) : true) &&
		(valid_axis(z1y2, 0.1f) ? sat_test_axis(z1y2, &t1, &t2, &minOverlap, overlap) : true) &&
		(valid_axis(z1z2, 0.1f) ? sat_test_axis(z1z2, &t1, &t2, &minOverlap, overlap) : true)
	);
}

int triangle_clip(vec3 planeP, vec3 planeN, triangle* toClip, triangle* clipped1, triangle* clipped2) {
	planeN = vec3_normalize(planeN);

	vec4u* insidePoints[3] = { 0 }; size_t insideCount = 0;
	vec4u* outsidePoints[3] = { 0 }; size_t outsideCount = 0;

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
		clipped1->color = toClip->color;
		clipped1->symbol = toClip->symbol;

		clipped1->data[0] = *insidePoints[0];

		clipped1->data[1] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[0]->v3)}; clipped1->data[1].w = 1.0f;
		clipped1->data[2] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[1]->v3) }; clipped1->data[2].w = 1.0f;

		return 1;
	}

	if (insideCount == 2 && outsideCount == 1) {
		clipped1->color = toClip->color;
		clipped1->symbol = toClip->symbol;
		clipped2->color = toClip->color;
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
		},
		.xAxis = (vec4u){ .v3 = (vec3){ 1, 0, 0 } },
		.yAxis = (vec4u){ .v3 = (vec3){ 0, 1, 0 } },
		.zAxis = (vec4u){ .v3 = (vec3){ 0, 0, 1 } },
	};
}

bbox true_bbox(object* o) {
	return (bbox) {
		.data = {
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[0].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[1].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[2].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[3].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[4].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[5].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[6].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.data[7].v4) },
		},
		.xAxis = (vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.xAxis.v4) },
		.yAxis = (vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.yAxis.v4) },
		.zAxis = (vec4u) { .v4 = mat4_mul_vec4(&o->matrix, o->boundingBox.zAxis.v4) },
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