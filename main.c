#include "game.h"

int main(int argc, char* argv[]) {
	engine e = {
		.console = console_create(SCREEN_WIDTH, SCREEN_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT),

		.cbSetup = game_setup,
		.cbUpdate = game_update,
		.cbTeardown = game_teardown,

		.cbKeyDown = game_key_down,
		.cbKeyUp = game_key_up,
		.cbMouseBtn = game_mouse_btn,
		.cbMouseMove = game_mouse_move,
	};

	if (!e.console) return 1;
	engine_run(&e);
	console_destroy(e.console);

	return 0;
}