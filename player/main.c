#include <stdio.h>
#include "player.h"

extern struct server_t server;
extern int key_move;
int to_player;
int from_player;


int main() {
    if(connect() != 0){
        printf("Server is full\n");
        return 1;
    }
    sleep(1);
    char fifo_to_player[40];
    sprintf(fifo_to_player, "/home/wsj/Pulpit/siema/to_player%d", server.player.number);
    to_player = open(fifo_to_player,O_RDONLY);

    char fifo_from_player[40];
    sprintf(fifo_from_player, "/home/wsj/Pulpit/siema/from_player%d", server.player.number);
    from_player = open(fifo_from_player,O_WRONLY);
    initscr();

    pthread_t input_key;
    pthread_create(&input_key,NULL,player_action,NULL);


    while(1){
        if(key_move == 'q' || key_move=='Q')break;
        key_move='\0';
        if(read_information()!=0){
            printf("Error with reading data,%d\n",server.player.number);
            return 1;
        }
        server.player.player_PID = getpid();
        print_map();
        print_info();
        if(send_information(player_move())!=0){
            perror("Error with sending data\n");
            return 1;
        }
    }
    endwin();
    close(to_player);
    close(from_player);
    remove(fifo_to_player);
    remove(fifo_from_player);
    return 0;
}
