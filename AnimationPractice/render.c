#include "render.h"

#include "obj.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

struct r3_vertex *render_vertices = NULL;
int image[IMAGE_WIDTH][IMAGE_HEIGHT];

void
render_clear ()
{
  for (int i = 0; i < IMAGE_WIDTH; i++)
    for (int j = 0; j < IMAGE_HEIGHT; j++)
      image[i][j] = 0;
}

static void
put_pixel (int x, int y)
{
  if (x >= 0 && x < IMAGE_WIDTH && y >= 0 && y < IMAGE_HEIGHT)
    image[x][y]++;
}

float render_view_x, render_view_y, render_view_width, render_view_height;

void
compute_render_vertices (struct mat4 f)
{
  if (render_vertices) free (render_vertices);
  render_vertices = malloc (sizeof(struct r3_vertex) * obj_no_vertices);
  for (int i = 0; i < obj_no_vertices; i++)
    {
      struct r4_vertex tv = mat4_apply (f, obj_vertices[i]);
      render_vertices[i] = r4_to_r3 (tv);
      render_vertices[i].x -= render_view_x;
      render_vertices[i].y -= render_view_y;
      render_vertices[i].x *= IMAGE_WIDTH / render_view_width;
      render_vertices[i].y *= IMAGE_HEIGHT / render_view_height;
    }
}

void
naive_render_line (int x1, int y1, int x2, int y2)
{
  int dx = x2 - x1;
  int dy = y2 - y1;
  if (dx == 0 && dy == 0)
    {
      put_pixel (x1, y1);
      return;
    }
  if (abs (dx) >= abs (dy))
    {
      for (int x = MIN (x1, x2); x <= MAX (x1, x2); x++)
	{
	  int y = (y1 * dx + (x - x1) * dy) / dx;
	  put_pixel (x, y);
	}
    }
  else
    {
      for (int y = MIN (y1, y2); y <= MAX (y1, y2); y++)
	{
	  int x = (x1 * dy + (y - y1) * dx) / dy;
	  put_pixel (x, y);
	}
    }
}

void
dda_render_line (int x1, int y1, int x2, int y2)
{
  int tmp;
  if (x1 == x2 && y1 == y2)
    {
      put_pixel (x1, y1);
      return;
    }
  float dx = x2 - x1;
  float dy = y2 - y1;
  if (ABS (dx) >= ABS (dy))
    {
      if (x1 > x2)
	{
	  SWAP (x1, x2, tmp);
	  SWAP (y1, y2, tmp);
	}
      float y = y1;
      float m = dy / dx;
      for (int x = x1; x <= x2; x++)
	{
	  y += m;
	  put_pixel (x, y);
	}
    }
  else
    {
      if (y1 > y2)
	{
	  SWAP (x1, x2, tmp);
	  SWAP (y1, y2, tmp);
	}
      float x = x1;
      float m = dx / dy;
      for (int y = y1; y <= y2; y++)
	{
	  x += m;
	  put_pixel (x, y);
	}
    }
}

void
bresenham_render_line (int x1, int y1, int x2, int y2)
{
  int tmp;
  if (x1 > x2)
    {
      SWAP (x1, x2, tmp);
      SWAP (y1, y2, tmp);
      return;
    }
#define DO(action)				\
  int dx = x2 - x1;				\
  int dy = y2 - y1;				\
  assert(dx >= 0 && dy >= 0 && dx >= dy);       \
  int d = 2 * dy - dx;				\
  int two_dx = 2 * dx;				\
  int two_dy = 2 * dy;				\
  int y = y1;					\
  for(int x = x1; x <= x2; x++)			\
    {						\
      action;					\
      if (d > 0)				\
	{					\
	  y++;					\
	  d -= two_dx;				\
	}					\
      d += two_dy;				\
    }
  int sdx = x2 - x1;
  int sdy = y2 - y1;
  if (sdy >= 0)
    {
      if (sdx >= sdy)
	{
	  DO (put_pixel (x, y));
	}
      else
	{
	  SWAP (x1, y1, tmp);
	  SWAP (x2, y2, tmp);
	  DO (put_pixel(y, x));
	}
    }
  else
    {
      if (sdx >= -sdy)
	{
	  y1 = -y1;
	  y2 = -y2;
	  DO (put_pixel (x, -y));
	}
      else
	{
	  y1 = -y1;
	  y2 = -y2;
	  SWAP (x1, y1, tmp);
	  SWAP (x2, y2, tmp);
	  DO (put_pixel (y, -x));
	}
    }
}

void
print_image (FILE * file, int resolution)
{
  fprintf (file, "P3\n");
  fprintf (file, "%d %d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  fprintf (file, "%d\n", resolution);
  float factor = 0.4;

  for (int y = IMAGE_HEIGHT - 1; y >= 0; y--)
    for (int x = 0; x < IMAGE_WIDTH; x++)
      {
	int pixel = MIN(image[x][y] * factor, resolution);
	fprintf (file, "%d %d %d\n", pixel, pixel, pixel);
      }
}

void
render_faces_range_xy (void (*render_line) (int, int, int, int), int from, int to)
{
  for (int i = from; i <= to; i++)
    {
      struct face face = obj_faces[i];
      for (int j = 0; j < face.no_indices; j++)
	{
	  assert (face.indices[j] < obj_no_vertices);
	  struct r3_vertex v1 = render_vertices[face.indices[j]];
	  struct r3_vertex v2 =
	    j + 1 <
	    face.no_indices ? render_vertices[face.indices[j +
							   1]] :
	    render_vertices[face.indices[0]];
	  render_line (v1.x, v1.y, v2.x, v2.y);
	}
    }
}

struct render_faces_range_xy_args
{
  void (*render_line) (int, int, int, int);
  int from, to;
};
void *
render_faces_range_xy_pthread (void *arg_ptr)
{
  struct render_faces_range_xy_args *args = arg_ptr;
  render_faces_range_xy (args->render_line, args->from, args->to);
  free (args);
  return arg_ptr;
}

enum
{ MAX_THREADS = 100 };
void
render_xy_multithreaded (int num_threads,
			 void (*render_line) (int, int, int, int))
{
  assert (render_vertices);
  int faces = obj_no_faces;
  int rest = faces % num_threads;
  int per_thread_min = faces / num_threads;
  pthread_t threads[MAX_THREADS];
  int face_ptr = 0;
  for (int i = 0; i < num_threads; i++)
    {
      int todo = per_thread_min;
      if (rest)
	{
	  todo++;
	  rest--;
	}
      int from = face_ptr;
      int to = face_ptr + todo - 1;

      struct render_faces_range_xy_args *args = malloc (sizeof (struct render_faces_range_xy_args));
      args->render_line = render_line;
      args->from = from;
      args->to = to;
      int ret_val =
	pthread_create (&threads[i], NULL, render_faces_range_xy_pthread, args);
      if (ret_val != 0)
	{
	  printf ("error creating thread");
	  exit (1);
	}

      face_ptr += todo;
    }
  for (int i = 0; i < num_threads; i++)
    {
      void *retval;
      pthread_join (threads[i], &retval);
    }
}
