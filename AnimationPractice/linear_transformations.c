#include "linear_transformations.h"

#include <math.h>

struct mat4
mat4_make_rotation_by_x (float angle)
{
  float cos_angle = cos (angle);
  float sin_angle = sin (angle);
  struct mat4 answer = {{{1, 0        , 0         , 0},
			 {0, cos_angle, -sin_angle, 0},
			 {0, sin_angle, cos_angle , 0},
			 {0, 0        , 0         , 1}}};
  return answer;
}

struct mat4
mat4_make_rotation_by_y (float angle)
{
  float cos_angle = cos (angle);
  float sin_angle = sin (angle);
  struct mat4 answer = {{{cos_angle, 0, -sin_angle, 0},
			 {0, 1, 0, 0},
			 {sin_angle, 0, cos_angle , 0},
			 {0, 0        , 0         , 1}}};
  return answer;
}


struct mat4
mat4_make_identity ()
{
  struct mat4 answer = {{{1, 0, 0, 0},
			 {0, 1, 0, 0},
			 {0, 0, 1, 0},
			 {0, 0, 0, 1}}};
  return answer;
}

struct mat4
mat4_make_scale (float x, float y, float z)
{
  struct mat4 answer = {{{x, 0, 0, 0},
			 {0, y, 0, 0},
			 {0, 0, z, 0},
			 {0, 0, 0, 1}}};
  return answer;
}

struct mat4
mat4_make_perspective_xy (float z_plane)
{
  struct mat4 answer = {{{1, 0, 0, 0},
			 {0, 1, 0, 0},
			 {0, 0, 1, 0},
			 {0, 0, 1.f/z_plane, 0}}};
  return answer; 
}
  
struct mat4
mat4_make_translation (float x, float y, float z)
{
  struct mat4 answer = {{{1, 0, 0, x},
		    {0, 1, 0, y},
		    {0, 0, 1, z},
		    {0, 0, 0, 1}}};
  return answer;
}

struct mat4
mat4_multiply (struct mat4 a, struct mat4 b)
{
  struct mat4 answer;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      answer.at[i][j] = 0;
  for (int i = 0; i < 4; i++)
    for (int k = 0; k < 4; k++)
      for (int j = 0; j < 4; j++)
	answer.at[i][j] += a.at[i][k] * b.at[k][j];
  return answer;
}

struct r4_vertex
mat4_apply (struct mat4 f, struct r4_vertex v)
{
  float x = v.x, y = v.y, z = v.z, w = v.w;
  struct r4_vertex answer
    = {f.at[0][0] * x + f.at[0][1] * y + f.at[0][2] * z + f.at[0][3] * w,
    f.at[1][0] * x + f.at[1][1] * y + f.at[1][2] * z + f.at[1][3] * w,
    f.at[2][0] * x + f.at[2][1] * y + f.at[2][2] * z + f.at[2][3] * w,
    f.at[3][0] * x + f.at[3][1] * y + f.at[3][2] * z + f.at[3][3] * w};
  return answer;
}
