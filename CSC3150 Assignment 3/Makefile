CC=gcc
CFLAGS=-g -Wall -O2 -pipe

all: oops

oops: oops.o

clean:
	rm *.o *~ *.orig
	
style:
	astyle *.c *.h --style=kr -Up