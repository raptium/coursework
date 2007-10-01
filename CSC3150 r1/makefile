all: 
	gcc -c -g -O2 -pipe main.c
	gcc -o tsh *.o
style:
	astyle --style=kr -Up -s2 *.c

clean:
	rm *.orig *.o *.txt tsh
