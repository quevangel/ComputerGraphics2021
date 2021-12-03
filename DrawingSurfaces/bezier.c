#include "bezier.h"

#include <assert.h>

LL bez_comb[MAX_CTRL_POINTS+1][MAX_CTRL_POINTS+1];
int bez_control_points_n, bez_control_points_m;
struct r3_vertex bez_control_points[MAX_CTRL_POINTS][MAX_CTRL_POINTS];
int bez_sample_points_n, bez_sample_points_m;

static void compute_comb(int n, int k)
{
  assert(k >= 0 && k <= n);
  if (n == 0) { bez_comb[n][k] = (int)(k == 0); return; }
  bez_comb[n][k] = bez_C(n-1, k) + bez_C(n-1, k-1);
}

void bez_init()
{
  for (int n = 0; n <= MAX_CTRL_POINTS; n++)
    {
      for (int k = 0; k <= n; k++)
	{
	  compute_comb(n, k);
	}
    }
}

LL bez_C(int n, int k)
{
  if (k < 0 || k > n) return 0;
  return bez_comb[n][k];
}

LD bez_BEZ(int k, int n, LD u)
{
  return bez_C(n, k) * powl(u, k) * powl(1 - u, n - k);
}

struct r3_vertex bez_get_sample(LD u, LD v)
{
  struct r3_vertex ans = {0, 0, 0};
  for (int i = 0; i < bez_control_points_n; i++)
    {
      for (int j = 0; j < bez_control_points_m; j++)
	{
	  struct r3_vertex term = bez_control_points[i][j];
	  r3_scale(&term, bez_BEZ(i, bez_control_points_n-1, u));
	  r3_scale(&term, bez_BEZ(j, bez_control_points_m-1, v));
	  r3_add(&ans, term);
	}
    }
  return ans;
}

void bez_read (FILE *bez_file)
{
  fscanf (bez_file, "%d%d", &bez_sample_points_n,
	 &bez_sample_points_m);
  fscanf (bez_file, "%d%d", &bez_control_points_n,
	 &bez_control_points_m);
  for (int i = 0; i < bez_control_points_n; i++)
    for (int j = 0; j < bez_control_points_m; j++)
      {
	fscanf (bez_file, "%f%f%f",
	       &bez_control_points[i][j].x,
	       &bez_control_points[i][j].y,
	       &bez_control_points[i][j].z);
      }
}

void bez_write_obj (FILE *obj_file)
{
  for (int i = 0; i < bez_sample_points_n; i++)
    {
      for (int j = 0; j < bez_sample_points_m; j++)
	{
	  struct r3_vertex sample = 
		    bez_get_sample(i * (LD)1.0 / (bez_sample_points_n-1),
				   j * (LD)1.0 / (bez_sample_points_m-1));
	  fprintf  (obj_file, "v %f %f %f\n", sample.x, sample.y, sample.z);
	}
    }
  
  for (int i = 0; i < bez_sample_points_n - 1; i++)
    {
      for (int j = 0; j < bez_sample_points_m - 1; j++)
	{
	  int p00 = i*bez_sample_points_m + j;
	  int p01 = i*bez_sample_points_m + (j+1);
	  int p10 = (i+1)*bez_sample_points_m + j;
	  int p11 = (i+1)*bez_sample_points_m + (j+1);
	  p00++, p01++, p10++, p11++;
	  fprintf (obj_file, "f %d %d %d\n", p00, p01, p10);
	  fprintf (obj_file, "f %d %d %d\n", p01, p10, p11);
	}
    }
}
