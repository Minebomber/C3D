#include "game.h"

float horizontal_to_vertical_fov(float hFov, float aspect) {
	return degrees(2.0f * atanf(aspect * tanf(radians(hFov / 2.0f))));
}

bool game_setup(engine* e) {
	objects = vector_create(sizeof(object));

	object obj = object_create_from_obj("axis4.obj");
	obj.physics.fixed = true;
	obj.mesh.color = FG_RED;
	obj.transform.rotation = (vec3){ 0, 0, 0 };
	vector_append(&objects, &obj);

	obj = object_create_from_obj("ship.obj");
	obj.transform.position = (vec3){ 0, 60, 0};
	obj.physics.velocity = (vec3){ 0, -10, 0 };
	obj.mesh.color = FG_BLUE;
	obj.transform.scale = (vec3){ 5, 5, 5 };
	obj.physics.elasticity = 1.0f;
	obj.physics.mass = 500.0f;
	vector_append(&objects, &obj);

	obj = object_create_from_obj("ship.obj");
	obj.transform.position = (vec3) { 0, 30, 0 };
	obj.mesh.color = FG_GREEN;
	obj.physics.elasticity = 1.0f;
	obj.physics.velocity = (vec3){ 0, 10, 0 };
	obj.transform.scale = (vec3){ 0.5f, 0.5f, 0.5f };
	obj.physics.mass = 1.0f;
	vector_append(&objects, &obj);

	for (size_t i = 0; i < objects.length; i++) {
		object* o = (object*)vector_get(&objects, i);
		if (o->cbSetup) o->cbSetup(o, e);
	}

	float aspect = ((float)e->console->height * (float)e->console->chr_height) / ((float)e->console->width * (float)e->console->chr_width);
	vFov = horizontal_to_vertical_fov(H_FOV, aspect);

	float nA = cosf(radians(H_FOV / 2.0f));
	float nB = sinf(radians(H_FOV / 2.0f));
	float nC = cosf(radians(vFov / 2.0f));
	float nD = sinf(radians(vFov / 2.0f));

	clipNormalLeft = (vec3){ nA, 0.0f, nB };
	clipNormalRight = (vec3){ -nA, 0.0f, nB };
	clipNormalBottom = (vec3){ 0.0f, nC, nD };
	clipNormalTop = (vec3){ 0.0f, -nC, nD };

	projectionMatrix = mat4_projection(vFov, aspect, Z_NEAR, Z_FAR);

	cameraYaw = M_PI_2;
	cameraPos = (vec3){ -5.0f, 25.0f, -25.0f };
	update_camera();
	shouldUpdateView = true;

	e->drawMode = DM_SOLID;
	e->cullMode = FC_BACK;

	return true;
}

CHAR_INFO color_for(float l, WORD c) {
	WORD sym = 0;
	WORD clr = 0;
	int br = max(0, min(5, (int)(l * 6.0f)));
	switch (br) {
	case 0:	sym = PX_THREEQUARTERS; clr = c << 4; break;
	case 1: sym = PX_HALF; clr = c << 4; break;
	case 2: sym = PX_QUARTER; clr = c; break;
	case 3: sym = PX_HALF; clr = c; break;
	case 4: sym = PX_THREEQUARTERS; clr = c; break;
	case 5: sym = PX_SOLID; clr = c; break;
	default: break;
	}
	CHAR_INFO r = { 0 };
	r.Attributes = clr;
	r.Char.UnicodeChar = sym;
	return r;
}

bool game_update(engine* e, float dt) {
	runningTime += dt;

	for (size_t i = 0; i < objects.length; i++) {
		object* o = (object*)vector_get(&objects, i);
		//o->physics.acceleration = (vec3){ 0.0f, -10.0f, 0.0f };
	}

	process_movement(e, dt);

	for (size_t i = 0; i < objects.length; i++) {
		object* o = (object*)vector_get(&objects, i);
		if (o->cbUpdate) o->cbUpdate(o, e, dt);
	}

	process_collisions(e, dt);

	render_objects(e);
	return !key_state(VK_ESCAPE);
}

void process_collisions(engine* e, float dt) {
	for (size_t i = 0; i < objects.length; i++) {
		for (size_t j = i + 1; j < objects.length; j++) {
			object* o1 = (object*)vector_get(&objects, i);
			object* o2 = (object*)vector_get(&objects, j);
			if (o1->physics.fixed && o2->physics.fixed) continue; // no physics needed
			vec3 col = { 0 };
			if (objects_colliding_sat(o1, o2, &col)) {
				if (o1->physics.fixed || o2->physics.fixed) {
					if (o1->physics.fixed) { object* t = o1; o1 = o2; o2 = t; }
					// o1 == moving
					// o2 == fixed
					o1->transform.position = o1->physics.prevPosition;
					vec3 n = vec3_normalize(col);
					vec3 v = vec3_project(o1->physics.velocity, n);
					o1->physics.velocity = vec3_sub(o1->physics.velocity, vec3_mul_scalar(v, 1.0f + o1->physics.elasticity));
					object_update_matrix(o1);
				} else {
					// both moving
					o1->transform.position = o1->physics.prevPosition;
					o2->transform.position = o2->physics.prevPosition;
					vec3 n = vec3_normalize(col);
					float vd = vec3_dot(vec3_sub(o2->physics.velocity, o1->physics.velocity), n);
					float jn = (o1->physics.mass * o2->physics.mass) / (o1->physics.mass + o2->physics.mass) * (1.0f + (o1->physics.elasticity + o2->physics.elasticity) / 2.0f) * vd;
					o1->physics.velocity = vec3_add(o1->physics.velocity, vec3_mul_scalar(n, jn / o1->physics.mass));
					o2->physics.velocity = vec3_add(o2->physics.velocity, vec3_mul_scalar(n, -jn / o2->physics.mass));
					object_update_matrix(o1);
					object_update_matrix(o2);
				}
			}
		}
	}
}

void process_movement(engine* e, float dt) {

	if (key_state(VK_SPACE)) {
		camera_fly(MOV_SPEED * dt);
		shouldUpdateView = true;
	}
	if (key_state(VK_SHIFT)) {
		camera_fly(-MOV_SPEED * dt);
		shouldUpdateView = true;
	}
	if (key_state('W')) {
		camera_move(MOV_SPEED * dt);
		shouldUpdateView = true;
	}
	if (key_state('S')) {
		camera_move(-MOV_SPEED * dt);
		shouldUpdateView = true;
	}
	if (key_state('A')) {
		camera_strafe(MOV_SPEED * dt);
		shouldUpdateView = true;
	}
	if (key_state('D')) {
		camera_strafe(-MOV_SPEED * dt);
		shouldUpdateView = true;
	}
	if (key_state(VK_LEFT)) {
		cameraYaw -= ROT_SPEED * dt;
		update_camera();
		shouldUpdateView = true;
	}
	if (key_state(VK_RIGHT)) {
		cameraYaw += ROT_SPEED * dt;
		update_camera();
		shouldUpdateView = true;
	}

	if (key_state(VK_UP)) {
		cameraPitch += ROT_SPEED * dt;
		if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
		if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
		update_camera();
		shouldUpdateView = true;
	}

	if (key_state(VK_DOWN)) {
		cameraPitch -= ROT_SPEED * dt;
		if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
		if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
		update_camera();
		shouldUpdateView = true;
	}

	if (mouse_state(MBTN_LEFT)) {
		int dx = (int)(mousePos.x - mousePosOld.x);
		int dy = (int)(mousePos.y - mousePosOld.y);
		cameraYaw += dx * ROT_SPEED * dt;
		cameraPitch -= dy * ROT_SPEED * dt;
		if (cameraPitch >= M_PI_2) cameraPitch = M_PI_2 - 0.1f;
		if (cameraPitch <= -M_PI_2) cameraPitch = -M_PI_2 + 0.1f;
		update_camera();
		shouldUpdateView = true;
		mousePosOld = mousePos;
	}

	if (shouldUpdateView) {
		update_view_matrix();
		shouldUpdateView = false;
	}

	object* ship = vector_get(&objects, 1);
	if (key_state('I'))	ship->physics.acceleration.z += 20.0f;
	if (key_state('K'))	ship->physics.acceleration.z -= 20.0f;
	if (key_state('L'))	ship->physics.acceleration.x -= 20.0f;
	if (key_state('J'))	ship->physics.acceleration.x += 20.0f;
	if (key_state('U'))	ship->physics.acceleration.y -= 20.0f;
	if (key_state('O'))	ship->physics.acceleration.y += 30.0f;
}

void render_objects(engine* e) {
	vector rasterQueue = vector_create(sizeof(triangle));
	for (size_t o = 0; o < objects.length; o++) {
		object* obj = (object*)vector_get(&objects, o);
		for (size_t i = 0; i < obj->mesh.triangles.length; i++) {
			triangle modelTri = triangle_multiply_matrix(*(triangle*)vector_get(&obj->mesh.triangles, i), &obj->mesh.matrix, true);
			vec3 line1 = vec3_sub(modelTri.data[1].v3, modelTri.data[0].v3);
			vec3 line2 = vec3_sub(modelTri.data[2].v3, modelTri.data[0].v3);
			vec3 normal = vec3_cross(line1, line2);
			normal = vec3_normalize(normal);
			vec3 cameraRay = vec3_sub(modelTri.data[0].v3, cameraPos);
			if ((vec3_dot(normal, cameraRay) < 0.0f) == (e->cullMode == FC_BACK)) {
				//vec3 lightDir = vec3_normalize((vec3) { 0.0f, 1.0f, 1.0f });
				vec3 lightDir = vec3_mul_scalar(cameraDir, -1.0f);
				float dp = vec3_dot(normal, lightDir);
				//CHAR_INFO c = grey_pixel(min(max(0.1f, dp), 0.99f));
				CHAR_INFO c = color_for(min(max(0.1f, dp), 0.99f), obj->mesh.color);

				triangle viewTri = triangle_multiply_matrix(modelTri, &viewMatrix, true);
				viewTri.color = c.Attributes;
				viewTri.symbol = c.Char.UnicodeChar;

				triangle clipped[2] = { 0 };
				linked_list clipQueue = { 0 };
				list_push_back(&clipQueue, sizeof(triangle), &viewTri);
				size_t clipCount = 1;

				for (int p = 0; p < 6; p++) {
					size_t toAdd = 0;
					while (clipCount > 0) {
						list_node* front = list_pop_front(&clipQueue);
						triangle* test = (triangle*)&front->data;
						clipCount--;
						switch (p) {
						case 0: // near
							toAdd = triangle_clip((vec3) { 0.0f, 0.0f, Z_NEAR }, (vec3) { 0.0f, 0.0f, 1.0f }, test, & clipped[0], & clipped[1]);
							break;
						case 1: // far
							toAdd = triangle_clip((vec3) { 0.0f, 0.0f, Z_FAR }, (vec3) { 0.0f, 0.0f, -1.0f }, test, & clipped[0], & clipped[1]);
							break;
						case 2: // left
							toAdd = triangle_clip((vec3) { 0 }, clipNormalLeft, test, & clipped[0], & clipped[1]);
							break;
						case 3: // right
							toAdd = triangle_clip((vec3) { 0 }, clipNormalRight, test, & clipped[0], & clipped[1]);
							break;
						case 4: // bottom
							toAdd = triangle_clip((vec3) { 0 }, clipNormalBottom, test, & clipped[0], & clipped[1]);
							break;
						case 5: // top
							toAdd = triangle_clip((vec3) { 0 }, clipNormalTop, test, & clipped[0], & clipped[1]);
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
					triangle* clippedTri = (triangle*)&node->data;

					triangle projTri = triangle_multiply_matrix(*clippedTri, &projectionMatrix, true);

					projTri.color = clippedTri->color;
					projTri.symbol = clippedTri->symbol;

					projTri.data[0].x *= -1.0f;
					projTri.data[0].y *= -1.0f;
					projTri.data[1].x *= -1.0f;
					projTri.data[1].y *= -1.0f;
					projTri.data[2].x *= -1.0f;
					projTri.data[2].y *= -1.0f;

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
	}

	qsort(rasterQueue.data, rasterQueue.length, rasterQueue.itemSize, triangle_compare);

	for (size_t i = 0; i < rasterQueue.length; i++) {
		triangle* t = vector_get(&rasterQueue, i);
		if (e->drawMode & DM_SOLID)
			console_fill_triangle(
				e->console,
				(int)(t->data[0].x), (int)(t->data[0].y),
				(int)(t->data[1].x), (int)(t->data[1].y),
				(int)(t->data[2].x), (int)(t->data[2].y),
				t->symbol, t->color
			);
		if (e->drawMode & DM_WIREFRAME)
			console_triangle(
				e->console,
				(int)(t->data[0].x), (int)(t->data[0].y),
				(int)(t->data[1].x), (int)(t->data[1].y),
				(int)(t->data[2].x), (int)(t->data[2].y),
				e->wireColor ? PX_SOLID : t->symbol,
				e->wireColor ? e->wireColor : t->color
			);
	}

	vector_destroy(&rasterQueue);
}

void game_teardown(engine* e) {
	for (size_t i = 0; i < objects.length; i++)
		vector_destroy(&((object*)vector_get(&objects, i))->mesh.triangles);
	vector_destroy(&objects);
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
	mousePos.x = (float)x;
	mousePos.y = (float)y;
}

bool mouse_state(int m) {
	return (mouseStates & m) != 0;
}

bool key_state(int k) {
	return (k >= 0 && k < 256) && keyStates[k];
}

void update_view_matrix() {
	vec3 up = { 0.0f, 1.0f, 0.0f };
	vec3 target = vec3_add(cameraPos, cameraDir);
	mat4 cameraMatrix = mat4_point_at(cameraPos, target, up);
	viewMatrix = mat4_quick_inverse(&cameraMatrix);
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
	CHAR_INFO c = { 0 };
	c.Attributes = bg_col | fg_col;
	c.Char.UnicodeChar = sym;
	return c;
}

float rand_range(float min, float max) {
	float r = max - min;
	return min + (float)rand() / ((float)RAND_MAX / r);
}