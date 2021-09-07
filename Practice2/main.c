#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))
#define abs(a) ((a)<0?(-(a)):(a))

enum {
  IMAGE_WIDTH = 1000,
  IMAGE_HEIGHT = 1000,
  MAX_VERTICES = 4000000,
  MAX_FACES = 4000000,
  MAX_VERTICES_PER_FACE = 4,
};
int image[IMAGE_WIDTH][IMAGE_HEIGHT];

int no_vertices = 0;
struct vertex { float x, y, z; };
struct vertex* vertices;

int no_faces = 0;
struct face { int no_indices; int indices[MAX_VERTICES_PER_FACE]; };
struct face* faces;

void
insert_vertex(struct vertex vertex)
{
  if (no_vertices == MAX_VERTICES)
    {
      fprintf(stderr, "Over limit of vertices\n");
      exit(1);
    }
  vertices[no_vertices++] = vertex;
}
void
insert_face(struct face face)
{
  assert(face.no_indices <= MAX_VERTICES_PER_FACE);
  if (no_faces == MAX_FACES)
    {
      fprintf(stderr, "Over limit of faces\n");
      exit(1);
    }
  faces[no_faces].no_indices = face.no_indices;
  for(int i = 0; i < face.no_indices; i++)
    faces[no_faces].indices[i] = face.indices[i];
  no_faces++;
}

void
naive_render_line(int x1, int y1, int x2, int y2)
{
  int dx = x2 - x1;
  int dy = y2 - y1;
  image[x1][y1] = 1;
  if (dx == 0 && dy == 0) { return; }
  if (abs(dx) >= abs(dy))
    {
      for(int x = min(x1, x2); x <= max(x1, x2); x++)
	{
	  int y = (y1 * dx + (x - x1) * dy)/dx;
	  image[x][y] = 1;
	}
    }
  else
    {
      for(int y = min(y1, y2); y <= max(y1, y2); y++)
	{
	  int x = (x1 * dy + (y - y1) * dx)/dy;
	  image[x][y] = 1;
	}
    }
}

void
read_obj(FILE* file)
{
  char buffer[1000];
  int no_vertices = 0;
  int no_faces = 0;
  struct vertex vertex;
  struct face face;
  while (fscanf(file, " %s", buffer) == 1)
    {
      switch(buffer[0])
	{
	case 'v':
	  {
	    no_vertices++;
	    struct vertex vertex;
	    fscanf(file, " %f %f %f",
		   &vertex.x,
		   &vertex.y,
		   &vertex.z);
	    insert_vertex(vertex);
	  }
	  break;
	case 'f':
	  {
	    no_faces++;
	    face.no_indices = 0;
	    int index;
	    while (fscanf(file, " %d", &index) == 1)
	      {
		face.indices[face.no_indices++] = index - 1;
	      }
	    insert_face(face);
	  }
	  break;
	default:
	  break;
	}
    }
}

struct vertex2d { float x, y; };
struct vertex2d project_xy(struct vertex vertex)
{
  struct vertex2d v = {vertex.x, vertex.y};
  return v;
}
void
render_xy(FILE* file)
{
  fprintf(file, "P3\n");
  fprintf(file, "%d %d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  fprintf(file, "1\n");
  float lox = INFINITY, loy = INFINITY, hix = -INFINITY, hiy = -INFINITY;
  struct vertex2d* render_vertices = malloc(sizeof(struct vertex2d) * MAX_VERTICES);
  for(int i = 0; i < no_vertices; i++)
    {
      render_vertices[i] = project_xy(vertices[i]);
      lox = min(lox, render_vertices[i].x);
      hix = max(hix, render_vertices[i].x);
      loy = min(loy, render_vertices[i].y);
      hiy = max(hiy, render_vertices[i].y);
    }
  float width = hix - lox;
  float height = hiy - loy;
  float factor_x = 1000.f / width;
  float factor_y = 1000.f / height;
  float factor = min(factor_x, factor_y);
  for(int i = 0; i < no_vertices; i++)
    {
      assert(render_vertices[i].x >= lox);
      assert(render_vertices[i].y >= loy);
      render_vertices[i].x -= lox;
      render_vertices[i].y -= loy;
      assert(render_vertices[i].x >= 0);
      assert(render_vertices[i].y >= 0);
      render_vertices[i].x *= factor;
      render_vertices[i].y *= factor; 
    }
  for(int i = 0; i < no_faces; i++)
    {
      struct face face = faces[i];
      for(int j = 0; j < face.no_indices; j++)
	{
	  assert(face.indices[j] < no_vertices);
	  struct vertex2d v1 = render_vertices[face.indices[j]];
	  struct vertex2d v2 = j+1<face.no_indices? render_vertices[face.indices[j+1]] :
	    render_vertices[face.indices[0]];
	  naive_render_line(v1.x, v1.y, v2.x, v2.y);
	}
    }
  for(int x = 0; x < IMAGE_WIDTH; x++)
    {
      for(int y = 0; y < IMAGE_HEIGHT; y++)
	{
	  int pixel = image[x][y];
	  fprintf(file, "%d %d %d\n", pixel, pixel, pixel);
	}
    }
}

int main(int argc, char* argv[])
{
  vertices = malloc(sizeof(struct vertex) * MAX_VERTICES);
  faces = malloc(sizeof(struct face) * MAX_FACES);
  if (argc < 2) return 1;
  char* filename = argv[1];
  FILE* obj_file = fopen(filename, "r");
  FILE* output_file = fopen("out2.ppm", "w");
  read_obj(obj_file);
  render_xy(output_file);
}
