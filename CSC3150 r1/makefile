all: style 
	gcc -c -g -O3 main.c
	gcc -o tsh *.o
style:
	~/bin/astyle --style=kr -Up -s2 *.c

clean:
	rm *.orig *.o *.txt
