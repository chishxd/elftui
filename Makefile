elftui: main.c parser.c parser.h
	gcc main.c parser.c -lncurses -o elftui

run: elftui
	./elftui /bin/ls

clean:
	rm -f ./elftui

.PHONY: run clean
