#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "obj.h"
#include "render.h"
#include "linear_transformations.h"

void print_help (char *executable_name);

int
main (int argc, char *argv[])
{
  if (argc < 3)
    {
      assert (argc >= 1);
      print_help (argv[0]);
      return 1;
    }
  init_obj ();
  char *obj_filename = argv[1];
  char *ppm_filename = argv[2];
  FILE *obj_file = fopen (obj_filename, "r");
  read_obj_fast (obj_file);
  printf ("There are %d vertices and %d faces\n",
	  obj_no_vertices, obj_no_faces);
  float model_width = obj_max_x - obj_min_x;
  float model_height = obj_max_y - obj_min_y;
  float model_depth = obj_max_z - obj_min_z;
  printf ("w %f h %f d %f\n", model_width, model_height, model_depth);
  struct mat4 model_t
    = mat4_make_translation (-obj_min_x - model_width * 0.5f,
			     -obj_min_y - model_height * 0.5f,
			     -obj_min_z - model_depth * 0.5f);
  model_t = mat4_multiply (mat4_make_scale (1, 1, 1), model_t);
  struct mat4 trans_t = mat4_make_translation (0, 0, 1.5);
  float dt = 1.0 / 40.0;
  struct mat4 frame_rotation = mat4_make_rotation_by_y (dt * M_PI * 2.0);
  struct mat4 P = mat4_make_perspective_xy (1.0);
  render_view_x = -5;
  render_view_y = -5;
  render_view_width = 10;
  render_view_height = 10;
  for (int i = 0; i < 80; i++)
    {
      char frame_name[256];
      sprintf (frame_name, "%s-%03d.ppm", ppm_filename, i);
      FILE *frame_file = fopen (frame_name, "w");
      struct mat4 T = model_t;
      T = mat4_multiply (trans_t, T);
      T = mat4_multiply (P, T);
      compute_render_vertices (T);
      render_clear ();
      render_xy_multithreaded (6, bresenham_render_line);
      print_image (frame_file, 255);
      printf ("written %s\n", frame_name);
      fclose (frame_file);
      model_t = mat4_multiply (frame_rotation, model_t);
    }
  fclose (obj_file);
}

void
print_help (char *executable_name)
{
  printf ("Usage: %s <input-obj-file> <output-ppm-file>\n", executable_name);
}
