#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "obj.h"
#include "project.h"
#include "render.h"

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
  FILE *ppm_file = fopen (ppm_filename, "w");
  read_obj (obj_file);
  render_xy (ppm_file, bresenham_render_line);
  fclose (obj_file);
  fclose (ppm_file);
}

void
print_help (char *executable_name)
{
  printf ("Usage: %s <input-obj-file> <output-ppm-file>\n", executable_name);
}
