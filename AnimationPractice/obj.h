#pragma once

#include <stdio.h>

#include "geometry_definitions.h"

enum
{
  MAX_VERTICES = 4000000,
  MAX_FACES = 4000000,
  MAX_VERTICES_PER_FACE = 4,
};
struct face
{
  int no_indices;
  int indices[MAX_VERTICES_PER_FACE];
};

extern int obj_no_vertices;
extern struct r4_vertex *obj_vertices;
extern int obj_no_faces;
extern struct face *obj_faces;
extern float obj_min_x, obj_max_x, obj_min_y, obj_max_y, obj_min_z, obj_max_z;

void init_obj ();
void read_obj (FILE * file);
void read_obj_fast (FILE * file);
