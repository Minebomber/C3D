#include "vmath.h"

vec4 vector_add(vec4 u, vec4 v) {
	return (vec4) { u.x + v.x, u.y + v.y, u.z + v.z, 1.0f };
}
vec4 vector_sub(vec4 u, vec4 v) {
	return (vec4) { u.x - v.x, u.y - v.y, u.z - v.z, 1.0f };
}

vec4 vector_mul(vec4 v, float f) {
	return (vec4) { v.x* f, v.y* f, v.z* f, v.w };
}
vec4 vector_div(vec4 v, float f) {
	return (vec4) { v.x / f, v.y / f, v.z / f, v.w };
}

float vector_length(vec4 v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float fast_inv_sqrt(float n) {
	long i;
	float x2, y;
	const float threehalfs = 1.5f;
	x2 = n * 0.5f;
	y = n;
	i = *(long*)&y;
	i = 0x5f3759df - (i >> 1);
	y = y * (threehalfs - (x2 * y * y));
	return y;
}

vec4 vector_normalize(vec4 v) {
	float l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return (vec4) { v.x / l, v.y / l, v.z / l, v.w };
}
vec4 vector_scale_w(vec4 v) {
	return (vec4) { v.x / v.w, v.y / v.w, v.z / v.w, 1.0f };
}

float vector_dot(vec4 u, vec4 v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

vec4 vector_cross(vec4 u, vec4 v) {
	return (vec4) {
		u.y* v.z - u.z * v.y,
			u.z* v.x - u.x * v.z,
			u.x* v.y - u.y * v.x,
			1.0f
	};
}

vec4 matrix_multiply_vector(mat4 m, vec4 v) {
	return (vec4) {
		v.x* m.data[0][0] + v.y * m.data[1][0] + v.z * m.data[2][0] + v.w * m.data[3][0],
			v.x* m.data[0][1] + v.y * m.data[1][1] + v.z * m.data[2][1] + v.w * m.data[3][1],
			v.x* m.data[0][2] + v.y * m.data[1][2] + v.z * m.data[2][2] + v.w * m.data[3][2],
			v.x* m.data[0][3] + v.y * m.data[1][3] + v.z * m.data[2][3] + v.w * m.data[3][3]
	};
}

mat4 matrix_multiply_matrix(mat4 m, mat4 n) {
	mat4 o = { 0 };
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			o.data[r][c] = m.data[r][0] * n.data[0][c] +
			m.data[r][1] * n.data[1][c] +
			m.data[r][2] * n.data[2][c] +
			m.data[r][3] * n.data[3][c];
	return o;
}

mat4 matrix_identity() {
	return (mat4) {
		1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
	};
}

mat4 matrix_translation(float x, float y, float z) {
	mat4 m = { 0 };
	m.data[0][0] = 1.0f;
	m.data[1][1] = 1.0f;
	m.data[2][2] = 1.0f;
	m.data[3][3] = 1.0f;
	m.data[3][0] = x;
	m.data[3][1] = y;
	m.data[3][2] = z;
	return m;
}

mat4 matrix_rotation_x(float angle) {
	mat4 m = { 0 };
	m.data[0][0] = 1.0f;
	m.data[1][1] = cosf(angle);
	m.data[1][2] = sinf(angle);
	m.data[2][1] = -sinf(angle);
	m.data[2][2] = cosf(angle);
	m.data[3][3] = 1.0f;
	return m;
}

mat4 matrix_rotation_y(float angle) {
	mat4 m = { 0 };
	m.data[0][0] = cosf(angle);
	m.data[0][2] = sinf(angle);
	m.data[2][0] = -sinf(angle);
	m.data[1][1] = 1.0f;
	m.data[2][2] = cosf(angle);
	m.data[3][3] = 1.0f;
	return m;
}

mat4 matrix_rotation_z(float angle) {
	mat4 m = { 0 };
	m.data[0][0] = cosf(angle);
	m.data[0][1] = sinf(angle);
	m.data[1][0] = -sinf(angle);
	m.data[1][1] = cosf(angle);
	m.data[2][2] = 1.0f;
	m.data[3][3] = 1.0f;
	return m;
}

mat4 matrix_projection(float fov, float aspect, float zNear, float zFar) {
	mat4 m = { 0 };
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);
	m.data[0][0] = aspect * fovRad;
	m.data[1][1] = fovRad;
	m.data[2][2] = zFar / (zFar - zNear);
	m.data[3][2] = (-zFar * zNear) / (zFar - zNear);
	m.data[2][3] = 1.0f;
	m.data[3][3] = 0.0f;
	return m;
}

mat4 matrix_point_at(vec4 pos, vec4 target, vec4 up) {
	vec4 fwd = vector_normalize(vector_sub(target, pos));
	up = vector_normalize(vector_sub(up, vector_mul(fwd, vector_dot(up, fwd))));
	vec4 right = vector_cross(up, fwd);
	mat4 m = { 0 };
	m.data[0][0] = right.x;		m.data[0][1] = right.y;		m.data[0][2] = right.z;		m.data[0][3] = 0.0f;
	m.data[1][0] = up.x;		m.data[1][1] = up.y;		m.data[1][2] = up.z;		m.data[1][3] = 0.0f;
	m.data[2][0] = fwd.x;		m.data[2][1] = fwd.y;		m.data[2][2] = fwd.z;		m.data[2][3] = 0.0f;
	m.data[3][0] = pos.x;		m.data[3][1] = pos.y;		m.data[3][2] = pos.z;		m.data[3][3] = 1.0f;
	return m;
}

mat4 matrix_quick_inverse(mat4 m) {
	mat4 o = { 0 };
	o.data[0][0] = m.data[0][0];	o.data[0][1] = m.data[1][0];	o.data[0][2] = m.data[2][0];	o.data[0][3] = 0.0f;
	o.data[1][0] = m.data[0][1];	o.data[1][1] = m.data[1][1];	o.data[1][2] = m.data[2][1];	o.data[1][3] = 0.0f;
	o.data[2][0] = m.data[0][2];	o.data[2][1] = m.data[1][2];	o.data[2][2] = m.data[2][2];	o.data[2][3] = 0.0f;
	o.data[3][0] = -(m.data[3][0] * o.data[0][0] + m.data[3][1] * o.data[1][0] + m.data[3][2] * o.data[2][0]);
	o.data[3][1] = -(m.data[3][0] * o.data[0][1] + m.data[3][1] * o.data[1][1] + m.data[3][2] * o.data[2][1]);
	o.data[3][2] = -(m.data[3][0] * o.data[0][2] + m.data[3][1] * o.data[1][2] + m.data[3][2] * o.data[2][2]);
	o.data[3][3] = 1.0f;
	return o;
}

vec4 vector_intersect_plane(vec4 planeP, vec4 planeN, vec4 lineStart, vec4 lineEnd) {
	planeN = vector_normalize(planeN);
	float planeD = -vector_dot(planeN, planeP);
	float ad = vector_dot(lineStart, planeN);
	float bd = vector_dot(lineEnd, planeN);
	float t = (-planeD - ad) / (bd - ad);
	vec4 lineSegment = vector_sub(lineEnd, lineStart);
	vec4 intersect = vector_mul(lineSegment, t);
	return vector_add(lineStart, intersect);
}

float vector_dist_to_plane(vec4 planeP, vec4 planeN, vec4 v) {
	return (planeN.x * v.x + planeN.y * v.y + planeN.z * v.z - vector_dot(planeN, planeP));
}
