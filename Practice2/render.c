#include "render.h"

#include "project.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

struct vertex2d* render_vertices;
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
dda_render_line (int x1, int y1, int x2, int y2)
{
  int tmp;
  if (x1 == x2 && y1 == y2)
    {
      image[x1][y1]++;
      return;
    }
  float dx = x2 - x1;
  float adx = abs (dx);
  float dy = y2 - y1;
  float ady = abs (dy);
  if (abs (dx) >= abs (dy))
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
	  image[x][(int) y]++;
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
	  image[(int) x][y]++;
	}
    }
}

void
bresenham_render_line (int x1, int y1, int x2, int y2)
{
  if (x1 > x2)
    {
      bresenham_render_line (x2, y2, x1, y1);
      return;
    }
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
	  DO (image[x][y]++);
	}
      else
	{
	  int tmp;
	  SWAP (x1, y1, tmp);
	  SWAP (x2, y2, tmp);
	  DO (image[y][x]++);
	}
    }
  else
    {
      if (sdx >= -sdy)
	{
	  y1 = -y1;
	  y2 = -y2;
	  DO (image[x][-y]++);
	}
      else
	{
	  y1 = -y1;
	  y2 = -y2;
	  int tmp;
	  SWAP (x1, y1, tmp);
	  SWAP (x2, y2, tmp);
	  DO (image[y][-x]++);
	}
    }
}

void
print_image(FILE* file, int resolution)
{
  fprintf (file, "P3\n");
  fprintf (file, "%d %d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  fprintf (file, "%d\n", resolution);
  
  for (int y = IMAGE_HEIGHT - 1; y >= 0; y--)
    for (int x = 0; x < IMAGE_WIDTH; x++)
      {
	int pixel = MIN (image[x][y], resolution);
	fprintf (file, "%d %d %d\n", pixel, pixel, pixel);
      }
}

struct vertex2d *
get_render_vertices()
{
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
  float factor_x = IMAGE_WIDTH / width;
  float factor_y = IMAGE_HEIGHT / height;
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
  return render_vertices;
}

void
render_xy (void (*render_line) (int, int, int, int), struct vertex2d* render_vertices,
	   int from, int to)
{
  for (int i = from; i <= to; i++)
    {
      struct face face = faces[i];
      for (int j = 0; j < face.no_indices; j++)
	{
	  assert (face.indices[j] < no_vertices);
	  struct vertex2d v1 = render_vertices[face.indices[j]];
	  struct vertex2d v2 =
	    j + 1 <
	    face.no_indices ? render_vertices[face.indices[j +
							   1]] :
	    render_vertices[face.indices[0]];
	  render_line (v1.x, v1.y, v2.x, v2.y);
	}
    }
}
struct render_xy_args
{
  void (*render_line)(int, int, int, int);
  struct vertex2d* render_vertices;
  int from, to;
};
void*
render_xy_pthread(void* arg_ptr)
{
  struct render_xy_args *args = arg_ptr;
  render_xy(args->render_line, args->render_vertices, args->from, args->to);
  free(args);
  return NULL;
}

enum {MAX_THREADS = 100};
void
render_xy_multithreaded(int num_threads, void (*render_line) (int, int, int, int))
{
  int faces = no_faces;
  int rest = faces % num_threads;
  int per_thread_min = faces / num_threads;
  pthread_t threads[MAX_THREADS];
  int face_ptr = 0;
  struct vertex2d* render_vertices = get_render_vertices();
  for(int i = 0; i < num_threads; i++)
    {
      int todo = per_thread_min;
      if (rest)
	{
	  todo++;
	  rest--;
	}
      int from = face_ptr;
      int to = face_ptr + todo - 1;

      struct render_xy_args *args = malloc(sizeof(struct render_xy_args));
      args->render_line = render_line;
      args->render_vertices = render_vertices;
      args->from = from;
      args->to = to;
      int ret_val = pthread_create(&threads[i], NULL, render_xy_pthread, args);
      if (ret_val != 0) { printf("error creating thread"); exit(1); }
      
      face_ptr += todo;
    }
  for(int i = 0; i < num_threads; i++)
    {
      void* retval;
      pthread_join(threads[i], &retval);
    }
}
