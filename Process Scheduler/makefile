# Makefile for compiling and running individual C programs

CC = gcc
CFLAGS = -Wall

# List of source files
SRCS = scheduler.c

# List of executable targets (remove .c extension)
TARGETS = $(SRCS:.c=)

.PHONY: all clean run build rebuild

all: $(TARGETS)

scheduler: scheduler.c
	$(CC) $(CFLAGS) -o scheduler scheduler.c 

clean:
	rm -f $(TARGETS)
	
run: all
	./scheduler

build: all

rebuild: clean build
