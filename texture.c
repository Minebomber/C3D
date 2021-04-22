#include "texture.h"
#pragma warning(disable:4996)

texture texture_create_from_file(const char* path) {
	FILE* f = fopen(path, "r");
	texture t = { 0 };
	if (!fscanf(f, "%d", &t.width) || !fscanf(f, "%d", &t.height)) {
		printf("Invalid texture file\n");
		fclose(f);
		return (texture) { 0 };
	}

	t.data = (unsigned short*)malloc(sizeof(unsigned short) * t.width * t.height);

	for (int i = 0; i < t.width * t.height; i++) {
		if (!fscanf(f, "%hu", &t.data[i])) {
			printf("Texture read error\n");
			fclose(f);
			free(t.data);
			return (texture) { 0 };
		}
	}

	return t;
}

texture texture_create_from_color(unsigned short c) {
	texture t = (texture){
		.width = 1,
		.height = 1,
		.data = malloc(sizeof(unsigned short) * 4)
	};
	t.data[0] = c;
	t.data[1] = c;
	t.data[2] = c;
	t.data[3] = c;
	return t;
}

void texture_destroy(texture* t) {
	free(t->data);
	t->width = -1;
	t->height = -1;
	t->data = NULL;
}

unsigned short texture_get_xy(texture* t, size_t x, size_t y) {
	x = min(x, t->width - 1); x = max(x, 0);
	y = min(y, t->height - 1); y = max(y, 0);
	return t->data[y * t->width + x];
}
unsigned short texture_get_uv(texture* t, float u, float v) {
	u = min(u, 1.0f); u = max(u, 0.0f);
	v = min(v, 1.0f); v = max(v, 0.0f);
	return texture_get_xy(t, u * t->width, v * t->height);
}
