#pragma once
#include <stdbool.h>
#include "engine.h"
#include "vmath.h"
#include "mesh.h"
#include "vector.h"
#include "linked_list.h"

#define MOV_SPEED 16.0f
#define ROT_SPEED 2.0f

#define FOV 90.0f
#define Z_NEAR 0.1f
#define Z_FAR 500.0f

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

mat4 projectionMatrix;
mat4 viewMatrix;
mat4 modelMatrix;

mesh model;

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
//bool mouse_state(int m);

void update_view_matrix();
void update_model_matrix(float theta);

void update_camera();
void camera_move(float d);
void camera_strafe(float d);
void camera_fly(float d);

CHAR_INFO grey_pixel(float l);

bool game_setup(engine* e) {

	model = mesh_from_obj("mountains.obj");

	float aspect = ((float)e->console->height * (float)e->console->chr_height) / ((float)e->console->width * (float)e->console->chr_width);
	projectionMatrix = matrix_projection(FOV, aspect, Z_NEAR, Z_FAR);

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
			//needsViewUpdate = true;
		}
		if (key_state(VK_SHIFT)) {
			camera_fly(-MOV_SPEED * dt);
			//needsViewUpdate = true;
		}
		if (key_state('W')) {
			camera_move(MOV_SPEED * dt);
			//needsViewUpdate = true;
		}
		if (key_state('S')) {
			camera_move(-MOV_SPEED * dt);
			//needsViewUpdate = true;
		}
		if (key_state('A')) {
			camera_strafe(MOV_SPEED * dt);
			//needsViewUpdate = true;
		}
		if (key_state('D')) {
			camera_strafe(-MOV_SPEED * dt);
			//needsViewUpdate = true;
		}
		if (key_state(VK_LEFT)) {
			cameraYaw -= ROT_SPEED * dt;
			update_camera();
			//needsViewUpdate = true;
		}
		if (key_state(VK_RIGHT)) {
			cameraYaw += ROT_SPEED * dt;
			update_camera();
			//needsViewUpdate = true;
		}

		if (key_state(VK_UP)) {
			cameraPitch += ROT_SPEED * dt;
			if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
			if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
			update_camera();
			//needsViewUpdate = true;
		}
		if (key_state(VK_DOWN)) {
			cameraPitch -= ROT_SPEED * dt;
			if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
			if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
			update_camera();
			//needsViewUpdate = true;
		}
	}

	update_view_matrix();
	update_model_matrix(0);

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
			
			size_t clipCount = 0;
			triangle clipped[2];
			clipCount = triangle_clip(
				(vec4) { 0.0f, 0.0f, Z_NEAR, 1.0f },
				(vec4) { 0.0f, 0.0f, 1.0f, 1.0f },
				&viewTri,
				&clipped[0],
				&clipped[1]
			);
			for (size_t n = 0; n < clipCount; n++) {

				triangle projTri = triangle_multiply_matrix(clipped[n], projectionMatrix, false);
				
				if (triangle_passes_clip(projTri)) {

					projTri.data[0] = vector_scale_w(projTri.data[0]);
					projTri.data[1] = vector_scale_w(projTri.data[1]);
					projTri.data[2] = vector_scale_w(projTri.data[2]);

					projTri.color = clipped[n].color;
					projTri.symbol = clipped[n].symbol;

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
				}
			}
		}
	}

	qsort(rasterQueue.data, rasterQueue.length, rasterQueue.itemSize, triangle_compare);

	for (int i = 0; i < rasterQueue.length; i++) {
		triangle* toRaster = vector_get(&rasterQueue, i);

		triangle clipped[2];
		linked_list clipQueue = { 0 };
		list_push_back(&clipQueue, sizeof(triangle), toRaster);
		int newCount = 1;

		for (int p = 0; p < 4; p++) {
			int toAdd = 0;
			while (newCount > 0) {
				list_node* front = list_pop_front(&clipQueue);
				triangle* test = &front->data;
				newCount--;
				switch (p) {
				case 0: 
					toAdd = triangle_clip(
						(vec4) { 0.0f, 0.0f, 0.0f, 1.0f },
						(vec4) { 0.0f, 1.0f, 0.0f, 1.0f },
						test,
						&clipped[0],
						&clipped[1]
					); 
					break;
				case 1:
					toAdd = triangle_clip(
						(vec4) { 0.0f, (float)e->console->height - 1.0f, 0.0f, 1.0f },
						(vec4) { 0.0f, -1.0f, 0.0f, 1.0f },
						test,
						&clipped[0],
						&clipped[1]
					); 
					break;
				case 2:
					toAdd = triangle_clip(
						(vec4) { 0.0f, 0.0f, 0.0f, 1.0f },
						(vec4) { 1.0f, 0.0f, 0.0f, 1.0f },
						test,
						&clipped[0],
						&clipped[1]
					); 
					break;
				case 3:
					toAdd = triangle_clip(
						(vec4) { (float)e->console->width - 1.0f, 0.0f, 0.0f, 1.0f },
						(vec4) { -1.0f, 0.0f, 0.0f, 1.0f },
						test,
						&clipped[0],
						&clipped[1]
					); 
					break;
				}
				for (int w = 0; w < toAdd; w++) {
					list_push_back(&clipQueue, sizeof(triangle), &clipped[w]);
				}
				free(front);
			}
			newCount = clipQueue.length;
		}
		
		while (clipQueue.head) {
			list_node* node = list_pop_front(&clipQueue);
			triangle* t = &node->data;
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
				PX_SOLID, FG_BLACK
			);*/
			free(node);
		}
		list_destroy(&clipQueue);
	}

	vector_destroy(&rasterQueue);

	return !(GetAsyncKeyState(VK_ESCAPE) >> 16);
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
}

void game_mouse_move(engine* e, int x, int y) {
	mousePos.x = x;
	mousePos.y = y;
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