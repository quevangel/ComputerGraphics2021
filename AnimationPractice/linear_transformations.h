#pragma once
#include "geometry_definitions.h"

struct mat4
{
  float at[4][4];
};

struct mat4
mat4_make_rotation_by_x (float angle);

struct mat4
mat4_make_rotation_by_y (float angle);

struct mat4
mat4_make_identity ();

struct mat4
mat4_make_translation (float x, float y, float z);

struct mat4
mat4_make_perspective_xy (float z_plane);

struct mat4
mat4_make_scale (float x, float y, float z);

struct mat4
mat4_multiply (struct mat4, struct mat4);

struct r4_vertex
mat4_apply (struct mat4, struct r4_vertex);

