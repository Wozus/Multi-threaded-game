#ifndef INC_3_3_SERVER_H
#define INC_3_3_SERVER_H

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

#define MAP_X 53
#define MAP_Y 27

#define COLOR_S 1
#define COLOR_M 2
#define COLOR_C 3
#define COLOR_B 4
#define COLOR_P 5

#define MAX_PLAYERS 4

struct beast_t{
    int x;
    int y;

    pthread_mutex_t make_move;
    pthread_mutex_t read_move;
    int move;

    char last_block;
};
enum move{
    UP,
    DOWN,
    RIGHT,
    LEFT
};
struct droped_treasure_t{
    int x;
    int y;

    int amount;
    char last_block;
    unsigned char is_picked_up:1;
};

struct player_t{
    int number;
    pid_t playerPID;
    char * type;

    int x;
    int y;
    int deaths;
    unsigned char campsite :1;

    pthread_mutex_t make_move;
    //noszone
    int carried;
    //doniesione do campa
    int brought;

    //to_move
    enum move move_player;
    char last_block;

    unsigned char is_connected:1;
    int in_bush;

};

struct server_t{
    pid_t serverPID;
    struct map_t *map;

    int camp_x;
    int camp_y;

    int round_number;
    int treasures_on_map;


    int number_of_players;
    struct player_t player[4];

    int count_beast;
    struct beast_t beasts[15];

    int count_dropped_treasures;
    struct droped_treasure_t* dropped;

    pthread_t beast[100];

    int end;
};
struct map_t{
    int size_x;
    int size_y;

    char** map;
};

enum treasure{
    coin = 1,
    treasure = 10,
    big_treasure = 50
};
//function
struct map_t* read_map (const char * filename);
void print_map(struct map_t* map);
void print_server_info ();
int main_server (struct map_t * map);
void* server_actions();
void add_coin (char type);
void* beast_move (void *number);
void free_all();
void add_beast();
int can_go(int x,int y,enum move dir);
void start_summon();
void* try_to_connect();
void* main_communication(void* arg);
void add_player(int slot);
void clear_map();
int treasures(int slot,int x,int y);
void add_dropped_treasure(int amount,int x,int y,char last_block);
void beast_call(struct beast_t* beast);
#endif //INC_3_3_SERVER_H
