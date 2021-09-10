CC=gcc -Ofast -pedantic -std=c99 -lpthread
main: main.o obj.o render.o
	$(CC) main.o obj.o render.o -o main

main.o: main.c obj.h project.h render.h
	$(CC) -c main.c

obj.o: obj.c obj.h
	$(CC) -c obj.c

render.o: render.c render.h
	$(CC) -c render.c

indent:
	indent *c *h

clean:
	rm *o
