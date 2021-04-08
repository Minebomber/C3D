#pragma once
#include <time.h>
#include "console.h"

typedef struct engine {
	console* console;
	
	bool (*cbSetup)(struct engine*);
	bool (*cbUpdate)(struct engine*, float);
	void (*cbTeardown)(struct engine*);

	void (*cbKeyDown)(struct engine*, int);
	void (*cbKeyUp)(struct engine*, int);
	void (*cbMouseBtn)(struct engine*, int, bool);
	void (*cbMouseMove)(struct engine*, int, int);
} engine;

void engine_run(engine* engine) {
	if (engine->cbSetup && !engine->cbSetup(engine)) return;

	INPUT_RECORD inputBuffer[32];
	DWORD events = 0;

	WCHAR windowTitle[256];
	int mouseStates = 0;
	bool running = true;

	clock_t time1 = clock();
	clock_t time2;
	float frameTime = 0.0f;
	while (running) {
		// Process input
		GetNumberOfConsoleInputEvents(engine->console->in, &events);
		if (events > 0)
			ReadConsoleInput(engine->console->in, inputBuffer, sizeof(inputBuffer) / sizeof(INPUT_RECORD), &events);
		for (DWORD i = 0; i < events; i++) {
			INPUT_RECORD* e = inputBuffer + i;
			switch (e->EventType) {
			case KEY_EVENT:
				if (e->Event.KeyEvent.bKeyDown) {
					if (engine->cbKeyDown) { engine->cbKeyDown(engine, e->Event.KeyEvent.wVirtualKeyCode); }
				} else {
					if (engine->cbKeyUp) { engine->cbKeyUp(engine, e->Event.KeyEvent.wVirtualKeyCode); }
				}
				break;
			case MOUSE_EVENT:
				switch (e->Event.MouseEvent.dwEventFlags) {
				case 0:
				case DOUBLE_CLICK:
					if ((e->Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != (mouseStates & FROM_LEFT_1ST_BUTTON_PRESSED))
						if (engine->cbMouseBtn) engine->cbMouseBtn(engine, FROM_LEFT_1ST_BUTTON_PRESSED, e->Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED);

					if ((e->Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) != (mouseStates & FROM_LEFT_2ND_BUTTON_PRESSED))
						if (engine->cbMouseBtn) engine->cbMouseBtn(engine, FROM_LEFT_2ND_BUTTON_PRESSED, e->Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED);

					if ((e->Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED) != (mouseStates & FROM_LEFT_3RD_BUTTON_PRESSED))
						if (engine->cbMouseBtn) engine->cbMouseBtn(engine, FROM_LEFT_3RD_BUTTON_PRESSED, e->Event.MouseEvent.dwButtonState & FROM_LEFT_3RD_BUTTON_PRESSED);

					if ((e->Event.MouseEvent.dwButtonState & FROM_LEFT_4TH_BUTTON_PRESSED) != (mouseStates & FROM_LEFT_4TH_BUTTON_PRESSED))
						if (engine->cbMouseBtn) engine->cbMouseBtn(engine, FROM_LEFT_4TH_BUTTON_PRESSED, e->Event.MouseEvent.dwButtonState & FROM_LEFT_4TH_BUTTON_PRESSED);

					if ((e->Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != (mouseStates & RIGHTMOST_BUTTON_PRESSED))
						if (engine->cbMouseBtn) engine->cbMouseBtn(engine, RIGHTMOST_BUTTON_PRESSED, e->Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED);

					mouseStates = e->Event.MouseEvent.dwButtonState;
				case MOUSE_MOVED:
					if (engine->cbMouseMove) engine->cbMouseMove(engine, e->Event.MouseEvent.dwMousePosition.X, e->Event.MouseEvent.dwMousePosition.Y);
					break;
				}
			default:
				break;
			}
		}

		// --- Update ---
		console_clear(engine->console);

		// Calculate time
		time2 = clock();
		frameTime = (float)(time2 - time1) / (float)CLOCKS_PER_SEC;
		time1 = time2;

		// Perform
		if (engine->cbUpdate && !engine->cbUpdate(engine, frameTime))
			running = false;

		// Draw
		swprintf(windowTitle, 256, L"Console 3D Engine | FPS: %3.2f ; MS: %3.2f", 1.0f / frameTime, (frameTime * 1000.0f) / 60.0f);
		SetConsoleTitle(windowTitle);
		console_display(engine->console);
		static float targetTime = 1000.0f / 60.0f;
		//Sleep(2);
	}

	if (engine->cbTeardown) engine->cbTeardown(engine);
}