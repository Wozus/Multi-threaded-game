FLG=-Wall -Wextra -pedantic

server: server.o main.o
	gcc -g $(FLG) $^ -o $@ -lncurses -pthread

main.o: main.c
	gcc -g -c $(FLG)  $< -o $@

server.o: server.c server.h
	gcc -g -c $(FLG) $< -o $@

.PHONY: clean

clean:
	-rm server.o server
