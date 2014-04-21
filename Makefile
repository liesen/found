fsevents:
	gcc -g -std=c99 debug.c parser.c pred.c tree.c found_fsevents.c -framework CoreServices -o found
