#pragma once
#include <stdbool.h>
#include "engine.h"
#include "vmath.h"
#include "mesh.h"
#include "vector.h"
#include "linked_list.h"

#define MOV_SPEED 8.0f
#define ROT_SPEED 1.0f

#define H_FOV 90.0f
#define Z_NEAR 0.1f
#define Z_FAR 100.0f

#define M_PI		3.14159265358979323846f	/* pi */
#define M_PI_2		1.57079632679489661923f	/* pi/2 */
#define M_PI_4		0.78539816339744830962f	/* pi/4 */
#define M_1_PI		0.31830988618379067154f	/* 1/pi */
#define M_2_PI		0.63661977236758134308f	/* 2/pi */

enum DRAW_MODE {
	DM_SOLID = 0x1,
	DM_WIREFRAME = 0x2,
};

enum FACE_CULL {
	FC_FRONT = 0x1,
	FC_BACK = 0x2,
};

enum MOUSE_BUTTONS {
	MBTN_LEFT = FROM_LEFT_1ST_BUTTON_PRESSED,
	MBTN_RIGHT = RIGHTMOST_BUTTON_PRESSED,
	MBTN_CENTER = FROM_LEFT_2ND_BUTTON_PRESSED,
	MBTN_EXTRA1 = FROM_LEFT_3RD_BUTTON_PRESSED,
	MBTN_EXTRA2 = FROM_LEFT_4TH_BUTTON_PRESSED
};

float vFov;

mat4 projectionMatrix;
mat4 viewMatrix;
mat4 modelMatrix;

mesh model;

vec4 clipNormalLeft, clipNormalRight, clipNormalBottom, clipNormalTop;

vec4 cameraPos, cameraDir, cameraStrafe;
float cameraYaw, cameraPitch;

bool keyStates[256];
unsigned int mouseStates;

vec2 mousePos, mousePosOld;

float runningTime;

bool game_setup(engine* e);
bool game_update(engine* e, float dt);
void game_teardown(engine* e);

void game_key_down(engine* e, int k);
void game_key_up(engine* e, int k);
void game_mouse_btn(engine* e, int m, bool p);
void game_mouse_move(engine* e, int x, int y);

bool key_state(int k);
bool mouse_state(int m);

void update_view_matrix();
void update_model_matrix(float theta);

void update_camera();
void camera_move(float d);
void camera_strafe(float d);
void camera_fly(float d);

CHAR_INFO grey_pixel(float l);

float radians(float d) {
	return d / 180.0f * M_PI;
}

float degrees(float r) {
	return r / M_PI * 180.0f;
}

float horizontal_to_vertical_fov(float hFov, float aspect) {
	return degrees(2.0f * atanf(aspect * tanf(radians(hFov / 2.0f))));
}

bool game_setup(engine* e) {

	model = mesh_from_obj("mountains.obj");

	float aspect = ((float)e->console->height * (float)e->console->chr_height) / ((float)e->console->width * (float)e->console->chr_width);
	vFov = horizontal_to_vertical_fov(H_FOV, aspect);

	float nA = cosf(radians(H_FOV / 2.0f));
	float nB = sinf(radians(H_FOV / 2.0f));
	float nC = cosf(radians(vFov / 2.0f));
	float nD = sinf(radians(vFov / 2.0f));

	clipNormalLeft = (vec4){ nA, 0.0f, nB };
	clipNormalRight = (vec4) { -nA, 0.0f, nB };
	clipNormalBottom = (vec4){ 0.0f, nC, nD };
	clipNormalTop = (vec4){ 0.0f, -nC, nD };

	projectionMatrix = matrix_projection(vFov, aspect, Z_NEAR, Z_FAR);

	cameraYaw = M_PI_2;
	update_camera();

	return true;
}

bool game_update(engine* e, float dt) {

	static bool needsViewUpdate = true;
	static bool needsModelUpdate = true;
	{
		if (key_state(VK_SPACE)) {
			camera_fly(MOV_SPEED * dt);
			needsViewUpdate = true;
		}
		if (key_state(VK_SHIFT)) {
			camera_fly(-MOV_SPEED * dt);
			needsViewUpdate = true;
		}
		if (key_state('W')) {
			camera_move(MOV_SPEED * dt);
			needsViewUpdate = true;
		}
		if (key_state('S')) {
			camera_move(-MOV_SPEED * dt);
			needsViewUpdate = true;
		}
		if (key_state('A')) {
			camera_strafe(MOV_SPEED * dt);
			needsViewUpdate = true;
		}
		if (key_state('D')) {
			camera_strafe(-MOV_SPEED * dt);
			needsViewUpdate = true;
		}
		if (key_state(VK_LEFT)) {
			cameraYaw -= ROT_SPEED * dt;
			update_camera();
			needsViewUpdate = true;
		}
		if (key_state(VK_RIGHT)) {
			cameraYaw += ROT_SPEED * dt;
			update_camera();
			needsViewUpdate = true;
		}

		if (key_state(VK_UP)) {
			cameraPitch += ROT_SPEED * dt;
			if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
			if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
			update_camera();
			needsViewUpdate = true;
		}

		if (key_state(VK_DOWN)) {
			cameraPitch -= ROT_SPEED * dt;
			if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
			if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
			update_camera();
			needsViewUpdate = true;
		}

		if (mouse_state(MBTN_LEFT)) {
			int dx = mousePos.x - mousePosOld.x;
			int dy = mousePos.y - mousePosOld.y;
			cameraYaw += dx * ROT_SPEED * dt / (float)e->console->chr_width;
			cameraPitch -= dy * ROT_SPEED * dt / (float)e->console->chr_height;
			if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
			if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
			update_camera();
			needsViewUpdate = true;
			mousePosOld = mousePos;
		}
	}

	if (needsViewUpdate) {
		update_view_matrix();
		needsViewUpdate = false;
	}

	if (needsModelUpdate) {
		update_model_matrix(0);
		needsModelUpdate = false;
	}

	runningTime += dt;

	vector rasterQueue = vector_create(sizeof(triangle));
	for (size_t i = 0; i < model.length; i++) {
		triangle tri = model.data[i];

		triangle modelTri = triangle_multiply_matrix(tri, modelMatrix, true);
		vec4 line1 = vector_sub(modelTri.data[1], modelTri.data[0]);
		vec4 line2 = vector_sub(modelTri.data[2], modelTri.data[0]);
		vec4 normal = vector_cross(line1, line2);
		normal = vector_normalize(normal);
		vec4 cameraRay = vector_sub(modelTri.data[0], cameraPos);
		if (vector_dot(normal, cameraRay) < 0.0f) {
			vec4 lightDir = vector_normalize((vec4) { 0.0f, 1.0f, -1.0f });
			float dp = vector_dot(normal, lightDir);
			CHAR_INFO c = grey_pixel(min(max(0.1f, dp), 0.99f));
			triangle viewTri = triangle_multiply_matrix(modelTri, viewMatrix, true);
			viewTri.color = c.Attributes;
			viewTri.symbol = c.Char.UnicodeChar;

			triangle clipped[2];
			linked_list clipQueue = { 0 };
			list_push_back(&clipQueue, sizeof(triangle), &viewTri);
			size_t clipCount = 1;

			for (int p = 0; p < 6; p++) {
				size_t toAdd = 0;
				while (clipCount > 0) {
					list_node* front = list_pop_front(&clipQueue);
					triangle* test = &front->data;
					clipCount--;
					switch (p) {
					case 0: // near
						toAdd = triangle_clip((vec4) { 0.0f, 0.0f, Z_NEAR }, (vec4) { 0.0f, 0.0f, 1.0f }, test, &clipped[0], &clipped[1]);
						break;
					case 1: // far
						toAdd = triangle_clip((vec4) { 0.0f, 0.0f, Z_FAR }, (vec4) { 0.0f, 0.0f, -1.0f }, test, &clipped[0], &clipped[1]);
						break;
					case 2: // left
						toAdd = triangle_clip((vec4) { 0 }, clipNormalLeft, test, &clipped[0], &clipped[1]);
						break;
					case 3: // right
						toAdd = triangle_clip((vec4) { 0 }, clipNormalRight, test, &clipped[0], &clipped[1]);
						break;
					case 4: // bottom
						toAdd = triangle_clip((vec4) { 0 }, clipNormalBottom, test, &clipped[0], &clipped[1]);
						break;
					case 5: // top
						toAdd = triangle_clip((vec4) { 0 }, clipNormalTop, test, &clipped[0], &clipped[1]);
						break;
					}
					for (size_t w = 0; w < toAdd; w++) {
						list_push_back(&clipQueue, sizeof(triangle), &clipped[w]);
					}
					free(front);
				}
				clipCount = clipQueue.length;
			}

			while (clipQueue.head) {
				list_node* node = list_pop_front(&clipQueue);
				triangle* clippedTri = &node->data;

				triangle projTri = triangle_multiply_matrix(*clippedTri, projectionMatrix, true);

				projTri.color = clippedTri->color;
				projTri.symbol = clippedTri->symbol;

				projTri.data[0].x *= -1.0f; projTri.data[0].y *= -1.0f;
				projTri.data[1].x *= -1.0f; projTri.data[1].y *= -1.0f;
				projTri.data[2].x *= -1.0f; projTri.data[2].y *= -1.0f;

				projTri.data[0].x += 1.0f; projTri.data[0].y += 1.0f;
				projTri.data[1].x += 1.0f; projTri.data[1].y += 1.0f;
				projTri.data[2].x += 1.0f; projTri.data[2].y += 1.0f;

				projTri.data[0].x *= 0.5f * (float)e->console->width;
				projTri.data[1].x *= 0.5f * (float)e->console->width;
				projTri.data[2].x *= 0.5f * (float)e->console->width;
				projTri.data[0].y *= 0.5f * (float)e->console->height;
				projTri.data[1].y *= 0.5f * (float)e->console->height;
				projTri.data[2].y *= 0.5f * (float)e->console->height;

				vector_append(&rasterQueue, &projTri);

				free(node);
			}

			list_destroy(&clipQueue);
		}
	}

	qsort(rasterQueue.data, rasterQueue.length, rasterQueue.itemSize, triangle_compare);

	for (int i = 0; i < rasterQueue.length; i++) {
		triangle* t = vector_get(&rasterQueue, i);
		console_fill_triangle(
			e->console,
			(int)(t->data[0].x), (int)(t->data[0].y),
			(int)(t->data[1].x), (int)(t->data[1].y),
			(int)(t->data[2].x), (int)(t->data[2].y),
			t->symbol, t->color
		);

		/*console_triangle(
			e->console,
			(int)(t->data[0].x), (int)(t->data[0].y),
			(int)(t->data[1].x), (int)(t->data[1].y),
			(int)(t->data[2].x), (int)(t->data[2].y),
			PX_SOLID, FG_WHITE
		);*/
	}

	vector_destroy(&rasterQueue);

	return !key_state(VK_ESCAPE);
}

void game_teardown(engine* e) {
	free(model.data);
}

void game_key_down(engine* e, int k) {
	if (k >= 0 && k < 256)
		keyStates[k] = true;
}

void game_key_up(engine* e, int k) {
	if (k >= 0 && k < 256)
		keyStates[k] = false;
}

void game_mouse_btn(engine* e, int m, bool p) {
	if (p) mouseStates |= m;
	else mouseStates &= ~m;

	if (m == MBTN_LEFT && p)
		mousePosOld = mousePos;
}

void game_mouse_move(engine* e, int x, int y) {
	mousePos.x = x;
	mousePos.y = y;
}

bool mouse_state(int m) {
	return (mouseStates & m) != 0;
}

bool key_state(int k) {
	return (k >= 0 && k < 256) && keyStates[k];
}

void update_view_matrix() {
	vec4 up = { 0.0f, 1.0f, 0.0f, 1.0f };
	vec4 target = vector_add(cameraPos, cameraDir);
	mat4 cameraMatrix = matrix_point_at(cameraPos, target, up);
	viewMatrix = matrix_quick_inverse(cameraMatrix);
}

void update_model_matrix(float theta) {
	modelMatrix = matrix_identity();
	modelMatrix = matrix_multiply_matrix(modelMatrix, matrix_rotation_z(theta));
	modelMatrix = matrix_multiply_matrix(modelMatrix, matrix_rotation_x(theta * 0.5f));
	modelMatrix = matrix_multiply_matrix(modelMatrix, matrix_translation(0.0f, 0.0f, 6.0f));

}

void update_camera() {
	cameraDir.x = cosf(cameraYaw) * cosf(cameraPitch);
	cameraDir.y = sinf(cameraPitch);
	cameraDir.z = sinf(cameraYaw) * cosf(cameraPitch);
	cameraStrafe.x = cosf(cameraYaw - M_PI_2);
	cameraStrafe.z = sinf(cameraYaw - M_PI_2);
}

void camera_move(float d) {
	cameraPos.x += d * cameraDir.x;
	cameraPos.y += d * cameraDir.y;
	cameraPos.z += d * cameraDir.z;
}

void camera_strafe(float d) {
	cameraPos.x += d * cameraStrafe.x;
	cameraPos.z += d * cameraStrafe.z;
}

void camera_fly(float d) {
	cameraPos.y += d;
}

CHAR_INFO grey_pixel(float l) {
	int pixel_bw = (int)(l * 13.0f);
	wchar_t sym = 0;
	unsigned short bg_col = 0, fg_col = 0;
	switch (pixel_bw) {
	case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PX_SOLID; break;

	case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PX_QUARTER; break;
	case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PX_HALF; break;
	case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PX_THREEQUARTERS; break;
	case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PX_SOLID; break;

	case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PX_QUARTER; break;
	case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PX_HALF; break;
	case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PX_THREEQUARTERS; break;
	case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PX_SOLID; break;

	case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PX_QUARTER; break;
	case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PX_HALF; break;
	case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PX_THREEQUARTERS; break;
	case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PX_SOLID; break;
	}
	CHAR_INFO c;
	c.Attributes = bg_col | fg_col;
	c.Char.UnicodeChar = sym;
	return c;
}