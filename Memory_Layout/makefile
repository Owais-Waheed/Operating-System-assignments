# Makefile for compiling and running individual C programs

CC = gcc
#CFLAGS = -Wall

# List of source files
SRCS = memorysystem.c

# List of executable targets (remove .c extension)
TARGETS = $(SRCS:.c=)

.PHONY: all clean run build rebuild

all: $(TARGETS)

memorysystem: memorysystem.c
	$(CC)  -o memorysystem memorysystem.c 

clean:
	rm -f $(TARGETS)
	
run: all
	./memorysystem

build: all

rebuild: clean build

#$(CFLAGS)
