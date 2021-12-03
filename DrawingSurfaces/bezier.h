#pragma once

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "geometry_definitions.h"

typedef long long LL;
typedef long double LD;

#define MAX_CTRL_POINTS 100

extern LL bez_comb[MAX_CTRL_POINTS+1][MAX_CTRL_POINTS+1];
extern int bez_control_points_n, control_points_m;
extern struct r3_vertex bez_control_points[MAX_CTRL_POINTS][MAX_CTRL_POINTS];
extern int bez_sample_points_n, bez_sample_points_m;

void bez_init();
LL bez_C(int n, int k);
LD bez_BEZ(int k, int n, LD u);
struct r3_vertex bez_get_sample(LD u, LD v);
void bez_read (FILE *bez_file);
void bez_write_obj (FILE *obj_file);
