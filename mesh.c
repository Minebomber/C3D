#include "mesh.h"

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

mesh mesh_from_obj(const char* path) {
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

	mesh m = {
		.matrix = mat4_identity(),
		.triangles = triangles,
	};

	return m;
}
