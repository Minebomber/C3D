#pragma once

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define CHAR_WIDTH 2
#define CHAR_HEIGHT 2

#define MOV_SPEED 8.0f
#define ROT_SPEED 1.0f

#define H_FOV 90.0f
#define Z_NEAR 0.1f
#define Z_FAR 100.0f

#define M_PI		3.14159265358979323846f	/* pi */
#define M_PI_2		1.57079632679489661923f	/* pi/2 */
#define M_PI_4		0.78539816339744830962f	/* pi/4 */
#define M_1_PI		0.31830988618379067154f	/* 1/pi */
#define M_2_PI		0.63661977236758134308f	/* 2/pi */

enum COLOR {
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE {
	PX_SOLID = 0x2588,
	PX_THREEQUARTERS = 0x2593,
	PX_HALF = 0x2592,
	PX_QUARTER = 0x2591,
};

enum DRAW_MODE {
	DM_SOLID	 = 0x1,
	DM_WIREFRAME = 0x2,
};

enum FACE_CULL {
	FC_FRONT	= 0x1,
	FC_BACK		= 0x2,
};

enum MOUSE_BUTTONS {
	MBTN_LEFT	= 0x0001,
	MBTN_RIGHT	= 0x0002,
	MBTN_CENTER = 0x0004,
	MBTN_EXTRA1 = 0x0008,
	MBTN_EXTRA2 = 0x0010
};