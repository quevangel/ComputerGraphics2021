#include "obj.h"

#include <assert.h>
#include <stdlib.h>

int no_vertices = 0;
struct vertex *vertices;
int no_faces = 0;
struct face *faces;

static void
insert_vertex (struct vertex vertex)
{
  if (no_vertices == MAX_VERTICES)
    {
      fprintf (stderr, "Over limit of vertices\n");
      exit (1);
    }
  vertices[no_vertices++] = vertex;
}

static void
insert_face (struct face face)
{
  assert (face.no_indices <= MAX_VERTICES_PER_FACE);
  if (no_faces == MAX_FACES)
    {
      fprintf (stderr, "Over limit of faces\n");
      exit (1);
    }
  faces[no_faces].no_indices = face.no_indices;
  for (int i = 0; i < face.no_indices; i++)
    faces[no_faces].indices[i] = face.indices[i];
  no_faces++;
}

void
init_obj ()
{
  vertices = malloc(sizeof(struct vertex) * MAX_VERTICES);
  faces = malloc(sizeof(struct face) * MAX_FACES);
}

void
read_obj (FILE * file)
{
  char buffer[1000];
  int no_vertices = 0;
  int no_faces = 0;
  struct vertex vertex;
  struct face face;
  while (fscanf (file, " %s", buffer) == 1)
    {
      switch (buffer[0])
	{
	case 'v':
	  {
	    no_vertices++;
	    struct vertex vertex;
	    fscanf (file, " %f %f %f", &vertex.x, &vertex.y, &vertex.z);
	    insert_vertex (vertex);
	  }
	  break;
	case 'f':
	  {
	    no_faces++;
	    face.no_indices = 0;
	    int index;
	    while (fscanf (file, " %d", &index) == 1)
	      {
		face.indices[face.no_indices++] = index - 1;
	      }
	    insert_face (face);
	  }
	  break;
	default:
	  break;
	}
    }
}
