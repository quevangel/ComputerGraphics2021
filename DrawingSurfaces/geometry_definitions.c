#include "geometry_definitions.h"

struct r3_vertex
r4_to_r3 (struct r4_vertex v)
{
  return (struct r3_vertex)
  { v.x / v.w, v.y / v.w, v.z / v.w };
}

void r3_scale (struct r3_vertex* v, float f)
{
  v->x *= f;
  v->y *= f;
  v->z *= f;
}

void r3_add (struct r3_vertex* v, struct r3_vertex w)
{
  v->x += w.x;
  v->y += w.y;
  v->z += w.z;
}
