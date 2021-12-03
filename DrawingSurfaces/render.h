#pragma once

#include <stdio.h>

#include "geometry_definitions.h"
#include "linear_transformations.h"

enum
{
  IMAGE_WIDTH = 1000,
  IMAGE_HEIGHT = 1000,
};
extern int image[IMAGE_WIDTH][IMAGE_HEIGHT];
extern struct r3_vertex *render_vertices;
extern float render_view_x, render_view_y, render_view_width,
  render_view_height;
#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))
#define ABS(a) ((a)<0?(-(a)):(a))
#define SWAP(a, b, tmp) do { tmp = a; a = b; b = tmp; } while (0)

void compute_render_vertices (struct mat4 f);
void render_faces_range_xy (void (*render_line) (int, int, int, int),
			    int from, int to);
void print_image (FILE * file, int resolution, float factor);
void render_xy_multithreaded (int num_threads,
			      void (*render_line) (int, int, int, int));
void naive_render_line (int, int, int, int);
void dda_render_line (int, int, int, int);
void bresenham_render_line (int, int, int, int);
void render_clear ();
