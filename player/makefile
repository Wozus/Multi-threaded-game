player: player.o main.o
	gcc -g -Wall -pedantic $^ -o $@ -lncurses -pthread

main.o: main.c
	gcc -g -c -Wall -pedantic $< -o $@

player.o: player.c player.h
	gcc -g -c -Wall -pedantic $< -o $@

.PHONY: clean

clean:
	-rm player.o player
