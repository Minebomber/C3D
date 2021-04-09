#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include "constants.h"

#define SWAP(x,y) do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

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
void console_draw(console* c, int x, int y, wchar_t chr, unsigned short color);
void console_line(console* c, int x0, int y0, int x1, int y1, wchar_t chr, unsigned short color);
void console_triangle(console* c, int x0, int y0, int x1, int y1, int x2, int y2, wchar_t chr, unsigned short color);
void console_hline(console* c, int sx, int ex, int ny, wchar_t chr, unsigned short color);
void console_fill_triangle(console* c, int x1, int y1, int x2, int y2, int x3, int y3, wchar_t chr, unsigned short color);

