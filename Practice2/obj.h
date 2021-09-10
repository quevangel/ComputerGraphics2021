#pragma once

#include <stdio.h>

enum
{
  MAX_VERTICES = 4000000,
  MAX_FACES = 4000000,
  MAX_VERTICES_PER_FACE = 4,
};
struct vertex
{
  float x, y, z;
};
struct face
{
  int no_indices;
  int indices[MAX_VERTICES_PER_FACE];
};

extern int no_vertices;
extern struct vertex *vertices;
extern int no_faces;
extern struct face *faces;

void init_obj ();
void read_obj (FILE * file);
void read_obj_fast (FILE * file);

