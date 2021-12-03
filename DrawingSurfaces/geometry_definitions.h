#pragma once

struct r4_vertex
{
  float x, y, z, w;
};

struct r3_vertex
{
  float x, y, z;
};

struct r2_vertex
{
  float x, y;
};

struct r3_vertex r4_to_r3 (struct r4_vertex);

void r3_scale (struct r3_vertex*, float);
void r3_add (struct r3_vertex*, struct r3_vertex);
