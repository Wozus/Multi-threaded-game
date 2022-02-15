#ifndef INC_3_3_PLAYER_PLAYER_H
#define INC_3_3_PLAYER_PLAYER_H

#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ncurses.h>

#define COLOR_S 1
#define COLOR_M 2
#define COLOR_C 3
#define COLOR_B 4
#define COLOR_P 5

enum move{
    UP,
    DOWN,
    RIGHT,
    LEFT
};

struct player_t{
    pid_t player_PID;

    int number;
    char* type;
    int pos_x;
    int pos_y;
    int deaths;
    //coins
    int found;
    int brought;
};

struct server_t{
    char player_map[5][5];

    int server_PID;
    int camp_x;
    int camp_y;
    int round_number;
    unsigned char is_connected :1;

    struct player_t player;

};


int connect();
int read_information();
int send_information(enum move player_move);
enum move player_move();
void * player_action();
void print_map();
void print_info();
#endif //INC_3_3_PLAYER_PLAYER_H
