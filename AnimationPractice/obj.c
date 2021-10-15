#include "obj.h"

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

int obj_no_vertices = 0;
struct r4_vertex *obj_vertices;
int obj_no_faces = 0;
struct face *obj_faces;

inline static void
insert_vertex (struct r4_vertex vertex)
{
  if (obj_no_vertices == MAX_VERTICES)
    {
      fprintf (stderr, "Over limit of vertices\n");
      exit (1);
    }
  obj_vertices[obj_no_vertices++] = vertex;
}

inline static void
insert_face (struct face face)
{
  assert (face.no_indices <= MAX_VERTICES_PER_FACE);
  if (obj_no_faces == MAX_FACES)
    {
      fprintf (stderr, "Over limit of faces\n");
      exit (1);
    }
  obj_faces[obj_no_faces].no_indices = face.no_indices;
  for (int i = 0; i < face.no_indices; i++)
    obj_faces[obj_no_faces].indices[i] = face.indices[i];
  obj_no_faces++;
}

void
init_obj ()
{
  obj_vertices = malloc (sizeof (struct r4_vertex) * MAX_VERTICES);
  obj_faces = malloc (sizeof (struct face) * MAX_FACES);
}

void
read_obj (FILE * file)
{
  char buffer[1000];
  struct r4_vertex vertex;
  vertex.w = 1;
  struct face face;
  while (fscanf (file, " %s", buffer) == 1)
    {
      switch (buffer[0])
	{
	case 'v':
	  {
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

static inline float
read_float (char **ptr)
{
  int intpart = 0;
  int fltpart = 0;
  int sgn = 1;
  if ((**ptr) == '-')
    {
      sgn = -1;
      (*ptr)++;
    }
  do
    {
      intpart *= 10;
      intpart += **ptr - '0';
      (*ptr)++;
    }
  while ((**ptr) && isdigit (**ptr));

  if (**ptr != '.')
    return intpart;
  (*ptr)++;
  int pow10 = 1;
  do
    {
      fltpart *= 10;
      fltpart += **ptr - '0';
      pow10 *= 10;
      (*ptr)++;
    }
  while (**ptr && isdigit (**ptr));
  return sgn * (intpart + (float) fltpart / pow10);
}

static inline float
read_float_fast (char **ptr)
{
  int sign = 1;
  int intpart = 0;
  int frcpart = 0;
  int frcpow = 1;
  int state = 0;
begin:
  switch (**ptr)
    {
    default:
      return sign * (intpart + (float) frcpart / frcpow);
    case '-':
      sign *= -1;
      break;
    case '+':
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      switch (state)
	{
	case 0:
	  intpart *= 10;
	  intpart += **ptr - '0';
	  break;
	case 1:
	  frcpart *= 10;
	  frcpart += **ptr - '0';
	  frcpow *= 10;
	  break;
	}
      break;
    case '.':
      state ^= 1;
      break;
    }
  (*ptr)++;
  goto begin;
}

static inline void
read_space (char **ptr)
{
begin:
  switch (**ptr)
    {
    case ' ':
    case '\t':
    case '\n':
      (*ptr)++;
      goto begin;
    default:
      return;
    }
}

static inline void
read_int (char **ptr, int *val)
{
  *val = 0;
begin:
  switch (**ptr)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      *val *= 10;
      *val += **ptr - '0';
      (*ptr)++;
      goto begin;
    default:
      return;
    }
}

#include <math.h>
float obj_min_x, obj_max_x, obj_min_y, obj_max_y, obj_min_z, obj_max_z;
void
read_obj_fast (FILE * file)
{
  obj_min_x = INFINITY, obj_max_x = -INFINITY;
  obj_min_y = INFINITY, obj_max_y = -INFINITY;
  obj_min_z = INFINITY, obj_max_z = -INFINITY;
  fseek (file, 0, SEEK_END);
  long size = ftell (file);
  rewind (file);
  char *filestr = malloc (size + 1);
  fread (filestr, 1, size, file);
  filestr[size] = 0;
  char *ptr = filestr;
  struct r4_vertex vertex;
  vertex.w = 1;
  struct face face;
  while (*ptr)
    {
      read_space (&ptr);
      if (*ptr == 0)
	break;
      switch (*ptr)
	{
	case 'v':
	  {
	    ptr++;
	    read_space (&ptr);
	    vertex.x = read_float_fast (&ptr);
	    if (vertex.x < obj_min_x)
	      obj_min_x = vertex.x;
	    if (vertex.x > obj_max_x)
	      obj_max_x = vertex.x;
	    read_space (&ptr);
	    vertex.y = read_float_fast (&ptr);
	    if (vertex.y < obj_min_y)
	      obj_min_y = vertex.y;
	    if (vertex.y > obj_max_y)
	      obj_max_y = vertex.y;
	    read_space (&ptr);
	    vertex.z = read_float_fast (&ptr);
	    if (vertex.z < obj_min_z)
	      obj_min_z = vertex.z;
	    if (vertex.z > obj_max_z)
	      obj_max_z = vertex.z;
	    insert_vertex (vertex);
	  }
	  break;
	case 'f':
	  {
	    ptr++;
	    read_space (&ptr);
	    face.no_indices = 0;
	    int index;
	    do
	      {
		read_int (&ptr, &index);
		face.indices[face.no_indices++] = index - 1;
		read_space (&ptr);
	      }
	    while (isdigit (*ptr));
	    insert_face (face);
	  }
	  break;
	default:
	  while (*ptr && !isspace (*ptr))
	    ptr++;
	  break;
	}
    }
  free (filestr);
}
