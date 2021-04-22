#include "game.h"

float horizontal_to_vertical_fov(float hFov, float aspect) {
	return degrees(2.0f * atanf(aspect * tanf(radians(hFov / 2.0f))));
}

bool game_setup(engine* e) {
	objects = vector_create(sizeof(object));

	object obj = object_create_with_texture("cube.obj", "simple.tex");
	obj.physics.fixed = true;
	obj.transform.rotation = (vec3){ 0, 0, 0 };
	obj.transform.position = (vec3){ 0, 0, 0 };
	vector_append(&objects, &obj);

	// Setup texture uvs for cube

	((triangle*)vector_get(&obj.mesh.triangles, 0))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 0))->texCoords[1] = (vec3u){ .x = 0, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 0))->texCoords[2] = (vec3u){ .x = 1, .y = 0, .z = 1 };
																						
	((triangle*)vector_get(&obj.mesh.triangles, 1))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 1))->texCoords[1] = (vec3u){ .x = 1, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 1))->texCoords[2] = (vec3u){ .x = 1, .y = 1, .z = 1 };
																						  
	((triangle*)vector_get(&obj.mesh.triangles, 2))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 2))->texCoords[1] = (vec3u){ .x = 0, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 2))->texCoords[2] = (vec3u){ .x = 1, .y = 0, .z = 1 };

	((triangle*)vector_get(&obj.mesh.triangles, 3))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 3))->texCoords[1] = (vec3u){ .x = 1, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 3))->texCoords[2] = (vec3u){ .x = 1, .y = 1, .z = 1 };
																						
	((triangle*)vector_get(&obj.mesh.triangles, 4))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 4))->texCoords[1] = (vec3u){ .x = 0, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 4))->texCoords[2] = (vec3u){ .x = 1, .y = 0, .z = 1 };

	((triangle*)vector_get(&obj.mesh.triangles, 5))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 5))->texCoords[1] = (vec3u){ .x = 1, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 5))->texCoords[2] = (vec3u){ .x = 1, .y = 1, .z = 1 };
																						  
	((triangle*)vector_get(&obj.mesh.triangles, 6))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 6))->texCoords[1] = (vec3u){ .x = 0, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 6))->texCoords[2] = (vec3u){ .x = 1, .y = 0, .z = 1 };

	((triangle*)vector_get(&obj.mesh.triangles, 7))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 7))->texCoords[1] = (vec3u){ .x = 1, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 7))->texCoords[2] = (vec3u){ .x = 1, .y = 1, .z = 1 };

	((triangle*)vector_get(&obj.mesh.triangles, 8))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 8))->texCoords[1] = (vec3u){ .x = 0, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 8))->texCoords[2] = (vec3u){ .x = 1, .y = 0, .z = 1 };

	((triangle*)vector_get(&obj.mesh.triangles, 9))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 9))->texCoords[1] = (vec3u){ .x = 1, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 9))->texCoords[2] = (vec3u){ .x = 1, .y = 1, .z = 1 };

	((triangle*)vector_get(&obj.mesh.triangles, 10))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 10))->texCoords[1] = (vec3u){ .x = 0, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 10))->texCoords[2] = (vec3u){ .x = 1, .y = 0, .z = 1 };

	((triangle*)vector_get(&obj.mesh.triangles, 11))->texCoords[0] = (vec3u){ .x = 0, .y = 1, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 11))->texCoords[1] = (vec3u){ .x = 1, .y = 0, .z = 1 };
	((triangle*)vector_get(&obj.mesh.triangles, 11))->texCoords[2] = (vec3u){ .x = 1, .y = 1, .z = 1 };

	//obj = object_create_with_color("ship.obj", FG_BLUE);
	//obj.transform.position = (vec3){ 0, 50, 0};
	//obj.physics.velocity = (vec3){ 0, 0, 0 };
	////obj.transform.scale = (vec3){ 5, 5, 5 };
	//obj.physics.elasticity = 0.8f;
	//obj.transform.rotation = (vec3){ 0, 0, 0 };
	//obj.physics.mass = 1.0f;
	//vector_append(&objects, &obj);
	//
	//obj = object_create_with_color("ship.obj", FG_GREEN);
	//obj.transform.position = (vec3) { 0, 20, 0 };
	//obj.physics.elasticity = 0.8f;
	//obj.physics.velocity = (vec3){ 0, 0, 0 };
	//obj.transform.scale = (vec3){ 1, 1, 1 };
	//obj.transform.rotation = (vec3){ 0, 0, 0 };
	//obj.physics.mass = 1.0f;
	//vector_append(&objects, &obj);

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
	cameraPos = (vec3){ 0.0f, 0.0f, -5.0f };
	update_camera();
	shouldUpdateView = true;

	e->drawMode = DM_SOLID;
	e->wireColor = FG_WHITE;
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
		o->physics.acceleration = (vec3){ 0.0f, -10.0f, 0.0f };
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
	if (key_state('I'))	ship->physics.velocity.z += 30.0f * dt;
	if (key_state('K'))	ship->physics.velocity.z -= 30.0f * dt;
	if (key_state('L'))	ship->physics.velocity.x -= 30.0f * dt;
	if (key_state('J'))	ship->physics.velocity.x += 30.0f * dt;
	if (key_state('U'))	ship->physics.velocity.y -= 30.0f * dt;
	if (key_state('O'))	ship->physics.velocity.y += 30.0f * dt;
}

void render_objects(engine* e) {
	vector rasterQueue = vector_create(sizeof(triangle));
	for (size_t o = 0; o < objects.length; o++) {
		object* obj = (object*)vector_get(&objects, o);
		for (size_t i = 0; i < obj->mesh.triangles.length; i++) {
			triangle modelTri = triangle_multiply_matrix(*(triangle*)vector_get(&obj->mesh.triangles, i), &obj->mesh.matrix);
			vec3 line1 = vec3_sub(modelTri.points[1].v3, modelTri.points[0].v3);
			vec3 line2 = vec3_sub(modelTri.points[2].v3, modelTri.points[0].v3);
			vec3 normal = vec3_cross(line1, line2);
			normal = vec3_normalize(normal);
			vec3 cameraRay = vec3_sub(modelTri.points[0].v3, cameraPos);
			if ((vec3_dot(normal, cameraRay) < 0.0f) == (e->cullMode == FC_BACK)) {
				//vec3 lightDir = vec3_normalize((vec3) { 0.0f, 1.0f, 1.0f });
				vec3 lightDir = vec3_mul_scalar(cameraDir, -1.0f);
				float dp = vec3_dot(normal, lightDir);
				//CHAR_INFO c = grey_pixel(min(max(0.1f, dp), 0.99f));
				CHAR_INFO c = color_for(min(max(0.1f, dp), 0.99f), obj->mesh.texture.data[0]);

				triangle viewTri = triangle_multiply_matrix(modelTri, &viewMatrix);
				viewTri.color = c.Attributes;
				viewTri.symbol = c.Char.UnicodeChar;
				viewTri.fragData.texture = &obj->mesh.texture;
				viewTri.fragData.lightNormalDot = min(max(0.1f, dp), 0.99f);

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

					triangle projTri = triangle_multiply_matrix(*clippedTri, &projectionMatrix);

					projTri.texCoords[0].x = projTri.texCoords[0].x / projTri.points[0].w;
					projTri.texCoords[1].x = projTri.texCoords[1].x / projTri.points[1].w;
					projTri.texCoords[2].x = projTri.texCoords[2].x / projTri.points[2].w;

					projTri.texCoords[0].y = projTri.texCoords[0].y / projTri.points[0].w;
					projTri.texCoords[1].y = projTri.texCoords[1].y / projTri.points[1].w;
					projTri.texCoords[2].y = projTri.texCoords[2].y / projTri.points[2].w;

					projTri.texCoords[0].z = 1.0f / projTri.points[0].w;
					projTri.texCoords[1].z = 1.0f / projTri.points[1].w;
					projTri.texCoords[2].z = 1.0f / projTri.points[2].w;

					triangle_scale_points(&projTri);

					projTri.color = clippedTri->color;
					projTri.symbol = clippedTri->symbol;

					projTri.points[0].x *= -1.0f;
					projTri.points[0].y *= -1.0f;
					projTri.points[1].x *= -1.0f;
					projTri.points[1].y *= -1.0f;
					projTri.points[2].x *= -1.0f;
					projTri.points[2].y *= -1.0f;

					projTri.points[0].x += 1.0f; projTri.points[0].y += 1.0f;
					projTri.points[1].x += 1.0f; projTri.points[1].y += 1.0f;
					projTri.points[2].x += 1.0f; projTri.points[2].y += 1.0f;

					projTri.points[0].x *= 0.5f * (float)e->console->width;
					projTri.points[1].x *= 0.5f * (float)e->console->width;
					projTri.points[2].x *= 0.5f * (float)e->console->width;
					projTri.points[0].y *= 0.5f * (float)e->console->height;
					projTri.points[1].y *= 0.5f * (float)e->console->height;
					projTri.points[2].y *= 0.5f * (float)e->console->height;

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
			textured_triangle(e->console,
						 (int)(t->points[0].x), (int)(t->points[0].y), t->texCoords[0].x, t->texCoords[0].y, t->texCoords[0].z,
						 (int)(t->points[1].x), (int)(t->points[1].y), t->texCoords[1].x, t->texCoords[1].y, t->texCoords[1].z,
						 (int)(t->points[2].x), (int)(t->points[2].y), t->texCoords[2].x, t->texCoords[2].y, t->texCoords[2].z,
						 t->fragData.texture, t->fragData.lightNormalDot);
		if (e->drawMode & DM_WIREFRAME)
			console_triangle(
				e->console,
				(int)(t->points[0].x), (int)(t->points[0].y),
				(int)(t->points[1].x), (int)(t->points[1].y),
				(int)(t->points[2].x), (int)(t->points[2].y),
				e->wireColor ? PX_SOLID : t->symbol,
				e->wireColor ? e->wireColor : t->color
			);			
	}

	vector_destroy(&rasterQueue);
}

void swapi(int* a, int* b) {
	int t = *a;
	*a = *b;
	*b = t;
}

void swapf(float* a, float* b) {
	float t = *a;
	*a = *b;
	*b = t;
}

void textured_triangle(console* c,
					   int x1, int y1, float u1, float v1, float w1,
					   int x2, int y2, float u2, float v2, float w2,
					   int x3, int y3, float u3, float v3, float w3,
					   texture* tex, float l) {
	
	if (y2 < y1) {
		swapi(&y1, &y2);
		swapi(&x1, &x2);
		swapf(&u1, &u2);
		swapf(&v1, &v2);
		swapf(&w1, &w2);
	}

	if (y3 < y1) {
		swapi(&y1, &y3);
		swapi(&x1, &x3);
		swapf(&u1, &u3);
		swapf(&v1, &v3);
		swapf(&w1, &w3);
	}

	if (y3 < y2) {
		swapi(&y2, &y3);
		swapi(&x2, &x3);
		swapf(&u2, &u3);
		swapf(&v2, &v3);
		swapf(&w2, &w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1) 		{
		for (int i = y1; i <= y2; i++) 			{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) 				{
				swapi(&ax, &bx);
				swapf(&tex_su, &tex_eu);
				swapf(&tex_sv, &tex_ev);
				swapf(&tex_sw, &tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j <= bx; j++) 				{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				WORD clr = texture_get_uv(tex, tex_u / tex_w, tex_v / tex_w);
				CHAR_INFO chr = color_for(l, clr);
				console_draw(c, j, i, chr.Char.UnicodeChar, chr.Attributes);

				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1) {
		for (int i = y2; i <= y3; i++) {
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) {
				swapi(&ax, &bx);
				swapf(&tex_su, &tex_eu);
				swapf(&tex_sv, &tex_ev);
				swapf(&tex_sw, &tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j <= bx; j++) 				{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				WORD clr = texture_get_uv(tex, tex_u / tex_w, tex_v / tex_w);
				CHAR_INFO chr = color_for(l, clr);
				console_draw(c, j, i, chr.Char.UnicodeChar, chr.Attributes);

				t += tstep;
			}
		}
	}
}

void game_teardown(engine* e) {
	for (size_t i = 0; i < objects.length; i++) {
		object* o = (object*)vector_get(&objects, i);
		if (o->cbTeardown) o->cbTeardown(o, e);
	}
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