elftui: main.c
	gcc main.c -lncurses -o elftui

run: elftui
	./elftui

clean:
	rm ./elftui

.PHONY: run clean
