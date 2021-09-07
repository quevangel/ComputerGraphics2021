#pragma once

#include "obj.h"

struct vertex2d
{
  float x, y;
};
static inline struct vertex2d
project_xy (struct vertex vertex)
{
  struct vertex2d v = { vertex.x, vertex.y };
  return v;
}
