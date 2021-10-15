#include "geometry_definitions.h"

struct r3_vertex
r4_to_r3 (struct r4_vertex v)
{
  return (struct r3_vertex)
  { v.x / v.w, v.y / v.w, v.z / v.w };
}
