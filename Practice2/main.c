#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "obj.h"
#include "project.h"
#include "render.h"

int
main (int argc, char *argv[])
{
  init_obj ();
  if (argc < 2)
    return 1;
  char *filename = argv[1];
  FILE *obj_file = fopen (filename, "r");
  FILE *output_file = fopen ("out2.ppm", "w");
  read_obj (obj_file);
  render_xy (output_file, naive_render_line);
}
