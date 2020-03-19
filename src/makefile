
CC=gcc
CFLAGS=-g -W -Wall -ansi -pedantic
LDFLAGS=
EXEC=mfile.out

all: $(EXEC)

mfile: mfile.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

mfile.o: mfile.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o: main.c mfile.h
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
