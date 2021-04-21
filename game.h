#pragma once
#include <stdbool.h>
#include "engine.h"
#include "vmath2.h"
#include "vector.h"
#include "linked_list.h"
#include "object.h"
#include "texture.h"

float vFov;

mat4 projectionMatrix;
mat4 viewMatrix;
vector objects;

vec3 clipNormalLeft, clipNormalRight, clipNormalBottom, clipNormalTop;

vec3 cameraPos, cameraDir, cameraStrafe;
float cameraYaw, cameraPitch;

bool keyStates[256];
unsigned int mouseStates;

vec2 mousePos, mousePosOld;

float runningTime;

bool shouldUpdateView;

bool game_setup(engine* e);
bool game_update(engine* e, float dt);
void game_teardown(engine* e);

void game_key_down(engine* e, int k);
void game_key_up(engine* e, int k);
void game_mouse_btn(engine* e, int m, bool p);
void game_mouse_move(engine* e, int x, int y);

void process_collisions(engine* e, float dt);
void process_movement(engine* e, float dt);
void render_objects(engine* e);

bool key_state(int k);
bool mouse_state(int m);

void update_view_matrix();

void update_camera();
void camera_move(float d);
void camera_strafe(float d);
void camera_fly(float d);

CHAR_INFO grey_pixel(float l);

float horizontal_to_vertical_fov(float hFov, float aspect);
float rand_range(float min, float max);