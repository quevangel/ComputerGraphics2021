#include "render.h"

#include "project.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

int image[IMAGE_WIDTH][IMAGE_HEIGHT];

void
naive_render_line (int x1, int y1, int x2, int y2)
{
  int dx = x2 - x1;
  int dy = y2 - y1;
  if (dx == 0 && dy == 0)
    {
      image[x1][y1]++;
      return;
    }
  if (abs (dx) >= abs (dy))
    {
      for (int x = MIN (x1, x2); x <= MAX (x1, x2); x++)
	{
	  int y = (y1 * dx + (x - x1) * dy) / dx;
	  image[x][y]++;
	}
    }
  else
    {
      for (int y = MIN (y1, y2); y <= MAX (y1, y2); y++)
	{
	  int x = (x1 * dy + (y - y1) * dx) / dy;
	  image[x][y]++;
	}
    }
}

void
bresenham_render_line (int x1, int y1, int x2, int y2)
{
  if (x1 > x2) { bresenham_render_line (x2, y2, x1, y1); return; }
#define DO(action)				\
  int dx = x2 - x1;				\
  int dy = y2 - y1;				\
  assert(dx >= 0 && dy >= 0 && dx >= dy);       \
  int d = 2 * dy - dx;				\
  int y = y1;					\
  for(int x = x1; x <= x2; x++)			\
    {						\
      action;					\
      if (d > 0)				\
	{					\
	  y++;					\
	  d -= 2 * dx;				\
	}					\
      d += 2 * dy;				\
    }
  int sdx = x2 - x1;
  int sdy = y2 - y1;
  if (sdy >= 0)
    {
      if (sdx >= sdy)
	{
	  DO(image[x][y]++);
	}
      else
	{
	  int tmp;
	  SWAP (x1, y1, tmp);
	  SWAP (x2, y2, tmp);
	  DO(image[y][x]++);
	}
    }
  else
    {
      if (sdx >= -sdy)
	{
	  y1 = -y1;
	  y2 = -y2;
	  DO(image[x][-y]++);
	}
      else
	{
	  y1 = -y1;
	  y2 = -y2;
	  int tmp;
	  SWAP (x1, y1, tmp);
	  SWAP (x2, y2, tmp);
	  DO(image[y][-x]++);
	}
    }
}


void
render_xy (FILE * file, void (*render_line) (int, int, int, int))
{
  int resolution = 255;
  fprintf (file, "P3\n");
  fprintf (file, "%d %d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  fprintf (file, "%d\n", resolution);
  float lox = INFINITY, loy = INFINITY, hix = -INFINITY, hiy = -INFINITY;
  struct vertex2d *render_vertices =
    malloc (sizeof (struct vertex2d) * MAX_VERTICES);
  for (int i = 0; i < no_vertices; i++)
    {
      render_vertices[i] = project_xy (vertices[i]);
      lox = MIN (lox, render_vertices[i].x);
      hix = MAX (hix, render_vertices[i].x);
      loy = MIN (loy, render_vertices[i].y);
      hiy = MAX (hiy, render_vertices[i].y);
    }
  float width = hix - lox;
  float height = hiy - loy;
  float factor_x = 1000.f / width;
  float factor_y = 1000.f / height;
  float factor = MIN (factor_x, factor_y);
  for (int i = 0; i < no_vertices; i++)
    {
      assert (render_vertices[i].x >= lox);
      assert (render_vertices[i].y >= loy);
      render_vertices[i].x -= lox;
      render_vertices[i].y -= loy;
      assert (render_vertices[i].x >= 0);
      assert (render_vertices[i].y >= 0);
      render_vertices[i].x *= factor;
      render_vertices[i].y *= factor;
    }
  for (int i = 0; i < no_faces; i++)
    {
      struct face face = faces[i];
      for (int j = 0; j < face.no_indices; j++)
	{
	  assert (face.indices[j] < no_vertices);
	  struct vertex2d v1 = render_vertices[face.indices[j]];
	  struct vertex2d v2 =
	    j + 1 <
	    face.no_indices ? render_vertices[face.
					      indices[j +
						      1]] :
	    render_vertices[face.indices[0]];
	  render_line (v1.x, v1.y, v2.x, v2.y);
	}
    }
  for (int x = 0; x < IMAGE_WIDTH; x++)
    {
      for (int y = 0; y < IMAGE_HEIGHT; y++)
	{
	  int pixel = MIN (image[x][y], resolution);
	  fprintf (file, "%d %d %d\n", pixel, pixel, pixel);
	}
    }
}
