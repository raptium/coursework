all: style
	gcc -c -g -Wall main.c
	gcc -o tsh *.o
style:
	astyle --style=kr -Up -s2 *.c

clean:
	rm *.orig *.o *.txt
