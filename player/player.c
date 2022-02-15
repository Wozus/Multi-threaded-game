#include "player.h"

struct server_t server;
char fifo_connect[] = "/home/wsj/Pulpit/siema/connect";
int key_move;

extern int to_player;
extern int from_player;

void * player_action(){
    while(1) {
        key_move = getch();
        if(key_move == 'q' || key_move== 'Q')break;
    }
    pthread_exit(NULL);
}
void print_map(){
    clear();
    WINDOW * map_win = newwin(27,53,1,3);

    start_color();
    init_pair(COLOR_S,COLOR_WHITE,COLOR_WHITE);
    init_pair(COLOR_B,COLOR_RED,COLOR_BLACK);
    init_pair(COLOR_C,COLOR_YELLOW,COLOR_GREEN);
    init_pair(COLOR_M,COLOR_BLACK,COLOR_YELLOW);
    init_pair(COLOR_P,COLOR_WHITE,COLOR_MAGENTA);

    for(int i=server.player.pos_y-2,ii=0;i<5+server.player.pos_y-2;i++,ii++) {
        for(int j=server.player.pos_x-2,jj=0;j<5+server.player.pos_x-2;j++,jj++) {

            switch (server.player_map[ii][jj]) {
                case '_':{
                    wattron(map_win, COLOR_PAIR(COLOR_S));
                    mvwprintw(map_win, i, j, "%c", server.player_map[ii][jj]);
                    wattroff(map_win, COLOR_PAIR(COLOR_S));
                }break;

                case 'O':{
                    wattron(map_win, COLOR_PAIR(COLOR_C));
                    mvwprintw(map_win, i, j,"%c",'A');
                    wattroff(map_win, COLOR_PAIR(COLOR_C));
                }break;

                case '*':{
                    wattron(map_win, COLOR_PAIR(COLOR_B));
                    mvwprintw(map_win, i, j,"%c", server.player_map[ii][jj]);
                    wattroff(map_win, COLOR_PAIR(COLOR_B));
                }break;

                case 't':case 'c':case'T':case'D':{
                    wattron(map_win, COLOR_PAIR(COLOR_M));
                    mvwprintw(map_win, i, j,"%c", server.player_map[ii][jj]);
                    wattroff(map_win, COLOR_PAIR(COLOR_M));
                }break;

                case '1':case '2':case '3':case '4':{
                    wattron(map_win, COLOR_PAIR(COLOR_P));
                    mvwprintw(map_win, i, j,"%c", server.player_map[ii][jj]);
                    wattroff(map_win, COLOR_PAIR(COLOR_P));
                }break;

                default:mvwprintw(map_win, i, j, "%c", server.player_map[ii][jj]);
            }
        }
    }
    wattron(map_win, COLOR_PAIR(COLOR_C));
    mvwprintw(map_win, server.camp_y, server.camp_x,"%c",'A');
    wattroff(map_win, COLOR_PAIR(COLOR_C));

    box(map_win,0,0);
    refresh();
    wrefresh(map_win);
}
void print_info(){

    WINDOW * info_win = newwin(21,30,1,60);

    mvwprintw(info_win,1,2,"%s :%d","Server PID",server.server_PID);
    mvwprintw(info_win,2,2,"%s :%d/%d","Camp X/Y",server.camp_x,server.camp_y);
    mvwprintw(info_win,3,2,"%s :%d","Round number",server.round_number);
    mvwprintw(info_win,7,3,"%s %d","Player",server.player.number+1);
    mvwprintw(info_win,9,3,"%s :%s","Type",server.player.type);
    mvwprintw(info_win,10,3,"%s :%d/%d","Current X/Y",server.player.pos_x,server.player.pos_y);
    mvwprintw(info_win,11,3,"%s :%d","Deaths",server.player.deaths);
    mvwprintw(info_win,13,3,"%s :%d","Coins found",server.player.found);
    mvwprintw(info_win,14,3,"%s :%d","Coins brought",server.player.brought);

    box(info_win,0,0);
    refresh();
    wrefresh(info_win);

    WINDOW * in_win = newwin(4,30,23,60);
    box(in_win,0,0);

    mvwprintw(in_win,1,1,"%s","Choose your action:");
    refresh();
    wrefresh(in_win);
    move(24,80);
}
int connect(){
    int fd = open(fifo_connect,O_RDONLY);
    if(read(fd,&server.player.number, sizeof(int))!=sizeof(int))return -1;
    close(fd);
    if(server.player.number >= 0 && server.player.number <=3)
        server.is_connected = 1;
    else
        return -1;

    return 0;
}
int read_information(){
    for(int i=0;i<5;i++) {
        for (int j = 0; j < 5; j++) {
            if (read(to_player, &server.player_map[i][j], sizeof(char)) != sizeof(char))
                break;
        }
    }
    if(read(to_player,&server.server_PID, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.camp_x, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.camp_y, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.round_number, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.player.pos_x, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.player.pos_y, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.player.deaths, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.player.found, sizeof(int)) != sizeof(int))
        return -1;
    if(read(to_player,&server.player.brought, sizeof(int)) != sizeof(int))
        return -1;

    server.player.type = strdup("HUMAN");

    return 0;
}
enum move player_move(){
        switch ((char)key_move) {
            case 'w':
            case 'W': {
                return UP;
            }
            case 's':
            case 'S': {
                return DOWN;
            }
            case 'a':
            case 'A': {
                return LEFT;
            }
            case 'd':
            case 'D': {
                return RIGHT;
            }
            default:return -1;
        }
}
int send_information(enum move player_move){
    if(write(from_player,&player_move, sizeof(int))!= sizeof(int))
        return -1;
    if(write(from_player,&server.player.player_PID, sizeof(int))!= sizeof(int))
        return -1;
    if(write(from_player,&server.player.number, sizeof(int))!= sizeof(int))
        return -1;
    return 0;
}


