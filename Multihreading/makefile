# Makefile for compiling and running individual C programs

CC = gcc
CFLAGS = -Wall

# List of source files
SRCS = singlethread.c multithread.c

# List of executable targets (remove .c extension)
TARGETS = $(SRCS:.c=)

.PHONY: all clean

all: $(TARGETS)

singlethread: singlethread.c
	$(CC) $(CFLAGS) -o singlethread singlethread.c

multithread: multithread.c
	$(CC) $(CFLAGS) -o multithread multithread.c -lpthread

clean:
	rm -f $(TARGETS)
