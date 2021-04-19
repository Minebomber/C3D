#pragma once
#include <time.h>
#include "console.h"

typedef struct engine {
	console* console;
	
	int drawMode;
	int cullMode;
	int wireColor;

	bool (*cbSetup)(struct engine*);
	bool (*cbUpdate)(struct engine*, float);
	void (*cbTeardown)(struct engine*);

	void (*cbKeyDown)(struct engine*, int);
	void (*cbKeyUp)(struct engine*, int);
	void (*cbMouseBtn)(struct engine*, int, bool);
	void (*cbMouseMove)(struct engine*, int, int);
} engine;

void engine_run(engine* engine);