#pragma once

#include <stdint.h>

typedef long long LL;
typedef long double LD;

#define MAX_CTRL_POINTS 100

LL comb[MAX_CTRL_POINTS+1][MAX_CTRL_POINTS+1];
LD control_points[MAX_CTRL_POINTS][MAX_CTRL_POINTS];

void compute_comb(int n, int k);
void init_combs();
LL C(int n, int k);
LD BEZ(int k, int n, LD u);
