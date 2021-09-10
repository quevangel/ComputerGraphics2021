CC=gcc -O2 -c -pedantic -std=c99
main: main.o obj.o render.o
	gcc main.o obj.o render.o -o main

main.o: main.c obj.h project.h render.h
	$(CC) main.c

obj.o: obj.c obj.h
	$(CC) obj.c

render.o: render.c render.h
	$(CC) render.c

indent:
	indent *c *h

clean:
	rm *o
