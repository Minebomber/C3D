#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>

#define SWAP(x,y) do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

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

typedef struct console {
	HANDLE out;
	HANDLE original;
	HANDLE in;
	int width;
	int height;
	int chr_width;
	int chr_height;
	SMALL_RECT window;
	CHAR_INFO* buffer;

} console;

console* console_create(int screen_width, int screen_height, int char_width, int char_height);
void console_destroy(console* c);
void console_display(console* c);
void console_clear(console* c);

void console_draw(console* c, int x, int y, wchar_t chr, unsigned short color) {
	if (x >= 0 && x < c->width && y >= 0 && y < c->height) {
		c->buffer[y * c->width + x].Char.UnicodeChar = chr;
		c->buffer[y * c->width + x].Attributes = color;
	}
}

void console_line(console* c, int x0, int y0, int x1, int y1, wchar_t chr, unsigned short color) {
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x1 - x0; dy = y1 - y0;
	dx1 = abs(dx);
	dy1 = abs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;
	if (dy1 <= dx1) {
		if (dx >= 0) {
			x = x0; y = y0; xe = x1;
		} else {
			x = x1; y = y1; xe = x0;
		}

		console_draw(c, x, y, chr, color);

		for (i = 0; x < xe; i++) {
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else {
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			console_draw(c, x, y, chr, color);
		}
	} else {
		if (dy >= 0) {
			x = x0; y = y0; ye = y1;
		} else {
			x = x1; y = y1; ye = y0;
		}

		console_draw(c, x, y, chr, color);

		for (i = 0; y < ye; i++) {
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else {
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			console_draw(c, x, y, chr, color);
		}
	}
}

void console_triangle(console* c, int x0, int y0, int x1, int y1, int x2, int y2, wchar_t chr, unsigned short color) {
	console_line(c, x0, y0, x1, y1, chr, color);
	console_line(c, x1, y1, x2, y2, chr, color);
	console_line(c, x2, y2, x0, y0, chr, color);
}

static void console_hline(console* c, int sx, int ex, int ny, wchar_t chr, unsigned short color) {
	for (int i = sx; i <= ex; i++) console_draw(c, i, ny, chr, color);
}

void console_fill_triangle(console* c, int x1, int y1, int x2, int y2, int x3, int y3, wchar_t chr, unsigned short color) {
	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
	if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
	if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; } else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; } else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
		SWAP(dx1, dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
		SWAP(dy2, dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; } else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
next2:
		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		console_hline(c, minx, maxx, y, chr, color);    // Draw line from min to max points found on the y
									 // Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; } else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		SWAP(dy1, dx1);
		changed1 = true;
	} else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; } else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
next4:

		if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
		console_hline(c, minx, maxx, y, chr, color);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}

console* console_create(int screen_width, int screen_height, int char_width, int char_height) {
	console* c = malloc(sizeof(console));
	c->original = GetStdHandle(STD_OUTPUT_HANDLE);
	c->in = GetStdHandle(STD_INPUT_HANDLE);
	c->out = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	c->width = screen_width;
	c->height = screen_height;
	c->chr_width = char_width;
	c->chr_height = char_height;
	c->buffer = calloc(screen_width * screen_height, sizeof(CHAR_INFO));
	c->window = (SMALL_RECT){ 0, 0, 1, 1 };

	SetConsoleWindowInfo(c->out, TRUE, &c->window);
	if (!SetConsoleScreenBufferSize(c->out, (COORD) { screen_width, screen_height })) {
		printf("SetConsoleScreenBufferSize failed - (%d)\n", GetLastError());
		goto console_create_error;
	}
	if (!SetConsoleActiveScreenBuffer(c->out)) {
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		goto console_create_error;
	}

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = char_width;
	cfi.dwFontSize.Y = char_height;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_DONTCARE;

	if (!SetCurrentConsoleFontEx(c->out, false, &cfi)) {
		printf("SetCurrentConsoleFontEx failed - (%d)\n", GetLastError());
		goto console_create_error;
	}
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(c->out, &csbi)) {
		printf("GetConsoleScreenBufferInfo failed - (%d)\n", GetLastError());
		goto console_create_error;
	}

	if (screen_width > csbi.dwMaximumWindowSize.X) {
		printf("Desired width > Max width - (%d > %d)\n", screen_width, csbi.dwMaximumWindowSize.X);
		goto console_create_error;
	}
	if (screen_height > csbi.dwMaximumWindowSize.Y) {
		printf("Desired height > Max height - (%d > %d)\n", screen_height, csbi.dwMaximumWindowSize.Y);
		goto console_create_error;
	}

	c->window = (SMALL_RECT){ 0, 0, screen_width - 1, screen_height - 1 };

	if (!SetConsoleWindowInfo(c->out, TRUE, &c->window)) {
		printf("SetConsoleWindowInfo failed - (%d)\n", GetLastError());
		goto console_create_error;
	}

	if (!SetConsoleMode(c->in, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) {
		printf("SetConsoleMode failed - (%d)\n", GetLastError());
		goto console_create_error;
	}

	if (!c->buffer) {
		printf("Allocate buffer failed\n");
		goto console_create_error;
	}

	return c;
console_create_error:
	console_destroy(c);
	return NULL;
}

void console_destroy(console* c) {
	if (!SetConsoleActiveScreenBuffer(c->original))
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
	CloseHandle(c->out);
	if (c->buffer) free(c->buffer);
	free(c);
}

void console_display(console* c) {
	WriteConsoleOutput(c->out, c->buffer, (COORD) { c->width, c->height }, (COORD) { 0, 0 }, & c->window);
}

void console_clear(console* c) {
	memset(c->buffer, 0, sizeof(CHAR_INFO) * c->width * c->height);
}