all:
	gcc -std=c99 debug.c parser.c pred.c tree.c found.c -framework CoreServices -o found

