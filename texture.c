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
		.data = malloc(sizeof(unsigned short))
	};
	t.data[0] = c;
	return t;
}

void texture_destroy(texture* t) {
	free(t->data);
	t->width = -1;
	t->height = -1;
	t->data = NULL;
}

unsigned short texture_get(texture* t, size_t u, size_t v) {
	return t->data[v * t->width + u];
}