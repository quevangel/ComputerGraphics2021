# Drawing Surfaces

This program renders bezier surfaces described by a mesh of control points and a
defined resolution in the number of vertices actually produced.

As I thought that a single PPM image would not describe very well the shape of the object I have reused my code for the animation practice and I actually produce 32 images that if are viewed in sequence show the rotation of the object by the Z axis.

# Compilation

To compile the program, just run:

```
make
```

# Usage

The above compilation produces the "main" executable, which is invoked like this:

```
./main <bezier file> <output-ppm>
```

The bezier file describes the control points and mesh resolution.  Output ppm is the name of the PPMs to be produced without the .ppm suffix.

# Run the Demo

I have included a demo bezier_file to show the input format and also produce some sample images.  To use, run:

```
./main bezier_file out
```

This will produce in little time 32 images named out-xxx.ppm which can be viewed and will show the rotating object described in the bezier_file.

# Bezier File Format

```
<points per mesh row> <points per mesh column>
<control points mesh no rows : n> <control points mesh no cols : m>
<point 1 1> ... <point 1 m>
<point 2 1> ... <point 2 m>
...
<point n 1> ... <point n m>
```

See bezier_file for an example.