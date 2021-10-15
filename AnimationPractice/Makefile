CC=gcc -Wall -pedantic -std=gnu11 -lpthread -O3 -lm -g
main: main.o obj.o render.o linear_transformations.o geometry_definitions.o
	$(CC) main.o obj.o render.o geometry_definitions.o linear_transformations.o -o main

main.o: main.c obj.h render.h
	$(CC) -c main.c

obj.o: obj.c obj.h
	$(CC) -c obj.c

render.o: render.c render.h obj.h
	$(CC) -c render.c

linear_transformations.o: linear_transformations.c linear_transformations.h
	$(CC) -c linear_transformations.c

geometry_definitions.o: geometry_definitions.c geometry_definitions.h
	$(CC) -c geometry_definitions.c

indent:
	indent *c *h

clean:
	rm *o
