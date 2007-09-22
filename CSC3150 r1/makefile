all: style
	gcc -c task.c
	gcc -o tsh *.o
style:
	~/bin/astyle --style=kr -Up -s2 *.c

clean:
	rm *.orig
	rm *.o
