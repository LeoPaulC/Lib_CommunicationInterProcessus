CC = gcc
CFLAGS = -Wall
LDFLAGS = $(CFLAGS)
LDLIBS = -Wall $(shell pkg-config --libs check)

all: check_fifo
check_fifo: check_mfifo.c check_mfifo.o ../src/mfile.o

test: check_fifo
	./$< 

clean:
	rm -f check_fifo *.o