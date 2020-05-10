CC = gcc
CFLAGS = -Wall 
LDFLAGS = $(CFLAGS)
LDLIBS = -Wall $(shell pkg-config --libs check)

all: check_mfifo
check_fifo: check_mfifo.o ../src/mfile.o 

test: check_mfifo
	./$< 

clean:
	rm -f check_fifo *.o