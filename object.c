#include "object.h"
#pragma warning(disable:4996)

object object_create_with_color(const char* path, unsigned short color) {
	vector t = triangles_from_obj(path);
	object o = {
		.cbSetup = object_setup,
		.cbUpdate = object_update,
		.cbTeardown = object_teardown,
		.mesh = {
			.triangles = t,
			.matrix = mat4_identity(),
			.boundingBox = bbox_for_triangles(&t),
			.texture = texture_create_from_color(color),
		},
		
		.transform = {
			.scale = {1.0f, 1.0f, 1.0f},
		},

	};
	return o;
}

void object_update(object* o, engine* e, float dt) {
	o->physics.prevPosition = o->transform.position;
	if (!o->physics.fixed) {
		o->physics.velocity = vec3_add(o->physics.velocity, vec3_mul_scalar(o->physics.acceleration, dt));
		o->transform.position = vec3_add(o->transform.position, vec3_mul_scalar(o->physics.velocity, dt));

		if (fabsf(o->transform.position.x) > 100.0f) o->transform.position.x *= -1.0f;
		if (fabsf(o->transform.position.y) > 100.0f) o->transform.position.y *= -1.0f;
		if (fabsf(o->transform.position.z) > 100.0f) o->transform.position.z *= -1.0f;
	}
	object_update_matrix(o);
}

void object_update_matrix(object* o) {
	o->mesh.matrix = mat4_scale(o->transform.scale);
	//o->matrix = mat4_mul_mat4(&o->matrix, mat4_rotation(o->rotation));
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_rotation_z(o->transform.rotation.z));
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_rotation_y(o->transform.rotation.y));
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_rotation_x(o->transform.rotation.x));
	o->mesh.matrix = mat4_mul_mat4(&o->mesh.matrix, mat4_translation(o->transform.position));
}

void object_setup(object* o, engine* e) {
	object_update_matrix(o);
	o->physics.prevPosition = o->transform.position;
}

void object_teardown(object* o, engine* e) {
	if (o->mesh.texture.data)
		texture_destroy(&o->mesh.texture);
	if (o->mesh.triangles.data)
		vector_destroy(&o->mesh.triangles);
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
		float v = vec3_dot(a, b->points[i].v3);
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

	float d0 = vec3_dist_to_plane(planeP, planeN, toClip->points[0].v3);
	float d1 = vec3_dist_to_plane(planeP, planeN, toClip->points[1].v3);
	float d2 = vec3_dist_to_plane(planeP, planeN, toClip->points[2].v3);

	if (d0 >= 0.0f) { insidePoints[insideCount++] = &toClip->points[0]; } else { outsidePoints[outsideCount++] = &toClip->points[0]; }

	if (d1 >= 0.0f) { insidePoints[insideCount++] = &toClip->points[1]; } else { outsidePoints[outsideCount++] = &toClip->points[1]; }

	if (d2 >= 0.0f) { insidePoints[insideCount++] = &toClip->points[2]; } else { outsidePoints[outsideCount++] = &toClip->points[2]; }

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

		clipped1->points[0] = *insidePoints[0];

		clipped1->points[1] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[0]->v3)}; clipped1->points[1].w = 1.0f;
		clipped1->points[2] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[1]->v3) }; clipped1->points[2].w = 1.0f;

		return 1;
	}

	if (insideCount == 2 && outsideCount == 1) {
		clipped1->color = toClip->color;
		clipped1->symbol = toClip->symbol;
		clipped2->color = toClip->color;
		clipped2->symbol = toClip->symbol;

		clipped1->points[0] = *insidePoints[0];
		clipped1->points[1] = *insidePoints[1];
		clipped1->points[2] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[0]->v3, outsidePoints[0]->v3) }; clipped1->points[2].w = 1.0f;

		clipped2->points[0] = *insidePoints[1];
		clipped2->points[1] = clipped1->points[2];
		clipped2->points[2] = (vec4u){ .v3 = vec3_intersect_plane(planeP, planeN, insidePoints[1]->v3, outsidePoints[0]->v3) }; clipped2->points[2].w = 1.0f;

		return 2;
	}

	return 0;
}

int triangle_compare(const void* a, const void* b) {
	const triangle* t1 = a;
	const triangle* t2 = b;
	float z1 = t1->points[0].z + t1->points[1].z + t1->points[2].z;
	float z2 = t2->points[0].z + t2->points[1].z + t2->points[2].z;
	if (z1 < z2) return 1;
	if (z1 > z2) return -1;
	return 0;
}

triangle triangle_multiply_matrix(triangle t, mat4* m, bool scale) {
	triangle u = {
		(vec4u) { .v4 = mat4_mul_vec4(m, t.points[0].v4) },
		(vec4u) { .v4 = mat4_mul_vec4(m, t.points[1].v4) },
		(vec4u) { .v4 = mat4_mul_vec4(m, t.points[2].v4) },
	};
	if (scale) {
		u.points[0].v4 = vec4_scale_w(u.points[0].v4);
		u.points[1].v4 = vec4_scale_w(u.points[1].v4);
		u.points[2].v4 = vec4_scale_w(u.points[2].v4);
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
			minX = t->points[0].x; maxX = t->points[0].x;
			minY = t->points[0].y; maxY = t->points[0].y;
			minZ = t->points[0].z; maxZ = t->points[0].z;
			first = false;
		}

		for (size_t j = 0; j < 3; j++) {
			if (t->points[j].x < minX) minX = t->points[j].x;
			if (t->points[j].x > maxX) maxX = t->points[j].x;
			if (t->points[j].y < minY) minY = t->points[j].y;
			if (t->points[j].y > maxY) maxY = t->points[j].y;
			if (t->points[j].z < minZ) minZ = t->points[j].z;
			if (t->points[j].z > maxZ) maxZ = t->points[j].z;
		}
	}
	return (bbox) {
		.points = {
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
		.points = {
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[0].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[1].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[2].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[3].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[4].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[5].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[6].v4) },
			(vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.points[7].v4) },
		},
		.xAxis = (vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.xAxis.v4) },
		.yAxis = (vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.yAxis.v4) },
		.zAxis = (vec4u) { .v4 = mat4_mul_vec4(&o->mesh.matrix, o->mesh.boundingBox.zAxis.v4) },
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