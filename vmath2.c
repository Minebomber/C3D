#include "vmath2.h"

float degrees(float r) {
	return r / M_PI * 180.0f;
}
float radians(float d) {
	return d / 180.0f * M_PI;
}

vec2 vec2_add(vec2 u, vec2 v) {
	return (vec2) { u.x + v.x, u.y + v.y };
}
vec3 vec3_add(vec3 u, vec3 v) {
	return (vec3) { u.x + v.x, u.y + v.y, u.z + v.z };
}
vec4 vec4_add(vec4 u, vec4 v) {
	return (vec4) { u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w };
}

vec2 vec2_sub(vec2 u, vec2 v) {
	return (vec2) { u.x - v.x, u.y - v.y };
}
vec3 vec3_sub(vec3 u, vec3 v) {
	return (vec3) { u.x - v.x, u.y - v.y, u.z - v.z };
}
vec4 vec4_sub(vec4 u, vec4 v) {
	return (vec4) { u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w };
}

vec2 vec2_mul_scalar(vec2 v, float f) {
	return (vec2) { v.x * f, v.y * f };
}
vec3 vec3_mul_scalar(vec3 v, float f) {
	return (vec3) { v.x * f, v.y * f, v.z * f };
}
vec4 vec4_mul_scalar(vec4 v, float f) {
	return (vec4) { v.x * f, v.y * f, v.z * f, v.w * f };
}

vec2 vec2_mul_vec2(vec2 u, vec2 v) {
	return (vec2) { u.x * v.x, u.y * v.y };
}
vec3 vec3_mul_vec3(vec3 u, vec3 v) {
	return (vec3) { u.x * v.x, u.y * v.y, u.z * v.z };
}
vec4 vec4_mul_vec4(vec4 u, vec4 v) {
	return (vec4) { u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w };
}

vec2 vec2_div_scalar(vec2 v, float f) {
	return (vec2) { v.x / f, v.y / f };
}
vec3 vec3_div_scalar(vec3 v, float f) {
	return (vec3) { v.x / f, v.y / f, v.z / f };
}
vec4 vec4_div_scalar(vec4 v, float f) {
	return (vec4) { v.x / f, v.y / f, v.z / f, v.w / f };
}

vec2 vec2_div_vec2(vec2 u, vec2 v) {
	return (vec2) { u.x / v.x, u.y / v.y };
}
vec3 vec3_div_vec3(vec3 u, vec3 v) {
	return (vec3) { u.x / v.x, u.y / v.y, u.z / v.z };
}
vec4 vec4_div_vec4(vec4 u, vec4 v) {
	return (vec4) { u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w };
}

float vec2_dot(vec2 u, vec2 v) {
	return u.x * v.x + u.y * v.y;
}
float vec3_dot(vec3 u, vec3 v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

vec3 vec3_cross(vec3 u, vec3 v) {
	return (vec3) {
		u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x,
	};
}

float vec2_length(vec2 v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}
float vec3_length(vec3 v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec2 vec2_normalize(vec2 v) {
	return vec2_div_scalar(v, vec2_length(v));
}
vec3 vec3_normalize(vec3 v) {
	return vec3_div_scalar(v, vec3_length(v));
}

vec4 vec4_scale_w(vec4 v) {
	return (vec4) {
		v.x / v.w,
		v.y / v.w,
		v.z / v.w,
		1.0f,
	};
}

vec3 vec3_project(vec3 u, vec3 v) {
	return vec3_mul_scalar(v, vec3_dot(u, v) / powf(vec3_length(v), 2.0f));
}

vec3 mat3_mul_vec3(mat3* m, vec3 v) {
	return (vec3) {
		v.x * m->data[0][0] + v.y * m->data[1][0] + v.z * m->data[2][0],
		v.x * m->data[0][1] + v.y * m->data[1][1] + v.z * m->data[2][1],
		v.x * m->data[0][2] + v.y * m->data[1][2] + v.z * m->data[2][2]
	};
}
vec4 mat4_mul_vec4(mat4* m, vec4 v) {
	return (vec4) {
		v.x * m->data[0][0] + v.y * m->data[1][0] + v.z * m->data[2][0] + v.w * m->data[3][0],
		v.x * m->data[0][1] + v.y * m->data[1][1] + v.z * m->data[2][1] + v.w * m->data[3][1],
		v.x * m->data[0][2] + v.y * m->data[1][2] + v.z * m->data[2][2] + v.w * m->data[3][2],
		v.x * m->data[0][3] + v.y * m->data[1][3] + v.z * m->data[2][3] + v.w * m->data[3][3]
	};
}

mat3 mat3_mul_mat3(mat3* m, mat3 n) {
	mat3 o = { 0 };
	for (int c = 0; c < 3; c++)
		for (int r = 0; r < 3; r++)
			o.data[r][c] = m->data[r][0] * n.data[0][c] +
			m->data[r][1] * n.data[1][c] +
			m->data[r][2] * n.data[2][c];
	return o;
}
mat4 mat4_mul_mat4(mat4* m, mat4 n) {
	mat4 o = { 0 };
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			o.data[r][c] = m->data[r][0] * n.data[0][c] +
			m->data[r][1] * n.data[1][c] +
			m->data[r][2] * n.data[2][c] +
			m->data[r][3] * n.data[3][c];
	return o;
}

mat3 mat3_identity() {
	mat3 m = { 0 };
	m.data[0][0] = 1.0f;
	m.data[1][1] = 1.0f;
	m.data[2][2] = 1.0f;
	return m;
}
mat4 mat4_identity() {
	mat4 m = { 0 };
	m.data[0][0] = 1.0f;
	m.data[1][1] = 1.0f;
	m.data[2][2] = 1.0f;
	m.data[3][3] = 1.0f;
	return m;
}

mat3 mat3_translation(vec2 v) {
	mat3 m = { 0 };
	m.data[0][0] = 1.0f;
	m.data[1][1] = 1.0f;
	m.data[2][2] = 1.0f;
	m.data[2][0] = v.x;
	m.data[2][1] = v.y;
	return m;
}
mat4 mat4_translation(vec3 v) {
	mat4 m = { 0 };
	m.data[0][0] = 1.0f;
	m.data[1][1] = 1.0f;
	m.data[2][2] = 1.0f;
	m.data[3][3] = 1.0f;
	m.data[3][0] = v.x;
	m.data[3][1] = v.y;
	m.data[3][2] = v.z;
	return m;
}

mat4 mat4_rotation_x(float f) {
	mat4 m = { 0 };
	m.data[0][0] = 1.0f;
	m.data[1][1] = cosf(f);
	m.data[1][2] = sinf(f);
	m.data[2][1] = -sinf(f);
	m.data[2][2] = cosf(f);
	m.data[3][3] = 1.0f;
	return m;
}

mat4 mat4_rotation_y(float f) {
	mat4 m = { 0 };
	m.data[0][0] = cosf(f);
	m.data[0][2] = -sinf(f);
	m.data[2][0] = sinf(f);
	m.data[1][1] = 1.0f;
	m.data[2][2] = cosf(f);
	m.data[3][3] = 1.0f;
	return m;
}

mat4 mat4_rotation_z(float f) {
	mat4 m = { 0 };
	m.data[0][0] = cosf(f);
	m.data[0][1] = sinf(f);
	m.data[1][0] = -sinf(f);
	m.data[1][1] = cosf(f);
	m.data[2][2] = 1.0f;
	m.data[3][3] = 1.0f;
	return m;
}

mat3 mat3_rotation(float f) {
	mat3 m = { 0 };
	m.data[0][0] = cosf(f);
	m.data[0][1] = -sinf(f);
	m.data[1][0] = sinf(f);
	m.data[1][1] = cosf(f);
	m.data[2][2] = 1.0f;
	return m;
}

mat4 mat4_rotation(vec3 v) {
	mat4 m = { 0 };
	m.data[0][0] = cosf(v.x) * cosf(v.y);	m.data[1][0] = cosf(v.x) * sinf(v.y) * sinf(v.z) - sinf(v.x) * cosf(v.z);		m.data[2][0] = cosf(v.x) * sinf(v.y) * cosf(v.z) + sinf(v.x) * sinf(v.z);
	m.data[0][1] = sinf(v.x) * cosf(v.y);	m.data[1][1] = sinf(v.x) * sinf(v.y) * sinf(v.z) + cosf(v.x) * cosf(v.z);		m.data[2][1] = sinf(v.x) * sinf(v.y) * cosf(v.z) - cosf(v.x) * sinf(v.z);
	m.data[0][2] = -sinf(v.y);				m.data[1][2] = cosf(v.y) * sinf(v.z);											m.data[2][2] = cosf(v.y) * cosf(v.z);
	m.data[3][3] = 1.0f;
	return m;
}

mat3 mat3_scale(vec2 v) {
	mat3 m = { 0 };
	m.data[0][0] = v.x;
	m.data[1][1] = v.y;
	m.data[2][2] = 1.0f;
	return m;
}
mat4 mat4_scale(vec3 v) {
	mat4 m = { 0 };
	m.data[0][0] = v.x;
	m.data[1][1] = v.y;
	m.data[2][2] = v.z;
	m.data[3][3] = 1.0f;
	return m;
}

mat4 mat4_projection(float fov, float aspect, float zNear, float zFar) {
	mat4 m = { 0 };
	float fovRad = 1.0f / tanf(radians(fov * 0.5f));
	m.data[0][0] = aspect * fovRad;
	m.data[1][1] = fovRad;
	m.data[2][2] = zFar / (zFar - zNear);
	m.data[3][2] = (-zFar * zNear) / (zFar - zNear);
	m.data[2][3] = 1.0f;
	m.data[3][3] = 0.0f;
	return m;
}
mat4 mat4_point_at(vec3 pos, vec3 target, vec3 up) {
	vec3 fwd = vec3_normalize(vec3_sub(target, pos));
	up = vec3_normalize(vec3_sub(up, vec3_mul_scalar(fwd, vec3_dot(up, fwd))));
	vec3 right = vec3_cross(up, fwd);
	mat4 m = { 0 };
	m.data[0][0] = right.x;		m.data[0][1] = right.y;		m.data[0][2] = right.z;		m.data[0][3] = 0.0f;
	m.data[1][0] = up.x;		m.data[1][1] = up.y;		m.data[1][2] = up.z;		m.data[1][3] = 0.0f;
	m.data[2][0] = fwd.x;		m.data[2][1] = fwd.y;		m.data[2][2] = fwd.z;		m.data[2][3] = 0.0f;
	m.data[3][0] = pos.x;		m.data[3][1] = pos.y;		m.data[3][2] = pos.z;		m.data[3][3] = 1.0f;
	return m;
}
mat4 mat4_quick_inverse(mat4* m) {
	mat4 o = { 0 };
	o.data[0][0] = m->data[0][0];	o.data[0][1] = m->data[1][0];	o.data[0][2] = m->data[2][0];	o.data[0][3] = 0.0f;
	o.data[1][0] = m->data[0][1];	o.data[1][1] = m->data[1][1];	o.data[1][2] = m->data[2][1];	o.data[1][3] = 0.0f;
	o.data[2][0] = m->data[0][2];	o.data[2][1] = m->data[1][2];	o.data[2][2] = m->data[2][2];	o.data[2][3] = 0.0f;
	o.data[3][0] = -(m->data[3][0] * o.data[0][0] + m->data[3][1] * o.data[1][0] + m->data[3][2] * o.data[2][0]);
	o.data[3][1] = -(m->data[3][0] * o.data[0][1] + m->data[3][1] * o.data[1][1] + m->data[3][2] * o.data[2][1]);
	o.data[3][2] = -(m->data[3][0] * o.data[0][2] + m->data[3][1] * o.data[1][2] + m->data[3][2] * o.data[2][2]);
	o.data[3][3] = 1.0f;
	return o;
}

vec3 vec3_intersect_plane(vec3 planeP, vec3 planeN, vec3 lineStart, vec3 lineEnd) {
	planeN = vec3_normalize(planeN);
	float planeD = -vec3_dot(planeN, planeP);
	float ad = vec3_dot(lineStart, planeN);
	float bd = vec3_dot(lineEnd, planeN);
	float t = (-planeD - ad) / (bd - ad);
	vec3 lineSegment = vec3_sub(lineEnd, lineStart);
	vec3 intersect = vec3_mul_scalar(lineSegment, t);
	return vec3_add(lineStart, intersect);
}
float vec3_dist_to_plane(vec3 planeP, vec3 planeN, vec3 v) {
	return (planeN.x * v.x + planeN.y * v.y + planeN.z * v.z - vec3_dot(planeN, planeP));
}