#pragma once

#include <stdio.h>

enum
{
  IMAGE_WIDTH = 1000,
  IMAGE_HEIGHT = 1000,
};
extern int image[IMAGE_WIDTH][IMAGE_HEIGHT];
#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))
#define ABS(a) ((a)<0?(-(a)):(a))
#define SWAP(a, b, tmp) do { tmp = a; a = b; b = tmp; } while (0)

void render_xy (FILE * file, void (*render_line) (int, int, int, int));

void naive_render_line (int, int, int, int);

void dda_render_line (int, int, int, int);

void bresenham_render_line (int, int, int, int);
