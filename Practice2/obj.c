#include "obj.h"

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

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
  vertices = malloc (sizeof (struct vertex) * MAX_VERTICES);
  faces = malloc (sizeof (struct face) * MAX_FACES);
}

void
read_obj (FILE * file)
{
  char buffer[1000];
  struct vertex vertex;
  struct face face;
  while (fscanf (file, " %s", buffer) == 1)
    {
      switch (buffer[0])
	{
	case 'v':
	  {
	    struct vertex vertex;
	    fscanf (file, " %f %f %f", &vertex.x, &vertex.y, &vertex.z);
	    insert_vertex (vertex);
	  }
	  break;
	case 'f':
	  {
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

float
read_float(char** ptr)
{
  long long intpart = 0;
  long long fltpart = 0;
  int sgn = 1;
  
  while (**ptr && isspace(**ptr))
    {
      char* nextchar = (*ptr) + 1;
      *ptr = nextchar;
    }

  while ((**ptr) && (**ptr) == '-')
    {
      sgn *= -1;
      char* nextchar = (*ptr) + 1;
      *ptr = nextchar;
    }
  
  while ((**ptr) && isdigit(**ptr))
    {
      intpart *= 10;
      intpart += **ptr - '0';
      char* nextchar = (*ptr) + 1;
      *ptr = nextchar;
    }
  if (**ptr != '.') return intpart;
  (*ptr)++;
  long long pow10 = 1;
  while (**ptr && isdigit(**ptr))
    {
      fltpart *= 10;
      fltpart += **ptr - '0';
      pow10 *= 10;
      char* nextchar = (*ptr) + 1;
      *ptr = nextchar;
    }
  return (intpart + (float)fltpart / (float)pow10)*((float)sgn);
}
int
read_int(char** ptr, int* val)
{
  *val = 0;
  while (**ptr && isspace(**ptr))
    {
      char* nextchar = (*ptr) + 1;
      *ptr = nextchar;
    }
  if (**ptr == 0 || !isdigit(**ptr)) return 0;
  while (**ptr && isdigit(**ptr))
    {
      *val *= 10;
      *val += **ptr - '0';
      *ptr = (*ptr) + 1;
    }
  return 1;
}

void
read_obj_fast (FILE * file)
{
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);
  char* filestr = malloc(size+1);
  fread(filestr, 1, size, file);
  filestr[size] = 0;
  char* ptr = filestr;
  struct vertex vertex;
  struct face face;
  while (*ptr)
    {
      int read_chars;
      while (*ptr && isspace(*ptr))
	{
	  ptr++;
	}
      if (*ptr == 0) break;
      switch (*ptr)
	{
	case 'v':
	  {
	    ptr++;
	    struct vertex vertex;
	    vertex.x = read_float(&ptr);
	    vertex.y = read_float(&ptr);
	    vertex.z = read_float(&ptr);
	    insert_vertex (vertex);
	  }
	  break;
	case 'f':
	  {
	    ptr++;
	    face.no_indices = 0;
	    int index;
	    while (read_int(&ptr, &index) == 1)
	      {
		face.indices[face.no_indices++] = index - 1;
	      }
	    insert_face (face);
	  }
	  break;
	default:
	  while (*ptr && !isspace(*ptr)) ptr++;
	  break;
	}
    }
  free(filestr);
}
