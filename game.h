#pragma once
#include <stdbool.h>
#include "engine.h"
#include "vmath.h"
#include "mesh.h"
#include "vector.h"
#include "linked_list.h"
#include "object.h"

float vFov;

mat4 projectionMatrix;
mat4 viewMatrix;
vector objects;

vec4 clipNormalLeft, clipNormalRight, clipNormalBottom, clipNormalTop;

vec4 cameraPos, cameraDir, cameraStrafe;
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

void process_collisions(engine* e);
void process_movement(engine* e, float dt);
void render_objects(engine* e);

bool key_state(int k);
bool mouse_state(int m);

void update_view_matrix();
void update_model_matrix(mesh* m, float rx, float ry, float rz, float px, float py, float pz);

void update_camera();
void camera_move(float d);
void camera_strafe(float d);
void camera_fly(float d);

CHAR_INFO grey_pixel(float l);

float radians(float d);
float degrees(float r);
float horizontal_to_vertical_fov(float hFov, float aspect);
float rand_range(float min, float max);