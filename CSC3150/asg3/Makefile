CC=gcc
CFLAGS=-g -Wall -pipe

all: oops

oops: oops.o

oops.o: oops.c oops.h

clean:
	rm *.o *~ *.orig
	
style:
	astyle *.c *.h --style=kr -Up
