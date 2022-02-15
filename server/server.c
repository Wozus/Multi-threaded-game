#include "server.h"

int server_action;
struct server_t server;
pthread_t communication[4];
struct map_t* read_map (const char * filename){
    if(filename==NULL)
        return NULL;

    struct map_t *map = (struct map_t*) calloc(1, sizeof(struct map_t));

    map->size_x = MAP_X;
    map->size_y = MAP_Y;

    FILE* file = fopen(filename,"r");

    if(file == NULL){
        return NULL;
    }
    map->map = (char**) calloc(map->size_y, sizeof(char*));

    for(int i=0;i<map->size_y;i++){
        map->map[i] = (char*)calloc(map->size_x, sizeof(char));
        fread(map->map[i],map->size_x, sizeof(char),file);
        fseek(file,1,SEEK_CUR);
    }
    fclose(file);
    return map;
}

void print_map(struct map_t* map){
    if(map == NULL)
        return;

    clear();
    WINDOW * map_win = newwin(27,53,1,3);

    start_color();
    init_pair(COLOR_S,COLOR_WHITE,COLOR_WHITE);
    init_pair(COLOR_B,COLOR_RED,COLOR_BLACK);
    init_pair(COLOR_C,COLOR_YELLOW,COLOR_GREEN);
    init_pair(COLOR_M,COLOR_BLACK,COLOR_YELLOW);
    init_pair(COLOR_P,COLOR_WHITE,COLOR_MAGENTA);

    for(int i=0;i<map->size_y;i++) {
        for(int j=0;j<map->size_x;j++) {

            switch (map->map[i][j]) {
                case '_':{
                    wattron(map_win, COLOR_PAIR(COLOR_S));
                    mvwprintw(map_win, i, j, "%c", map->map[i][j]);
                    wattroff(map_win, COLOR_PAIR(COLOR_S));
                }break;

                case 'O':{
                    wattron(map_win, COLOR_PAIR(COLOR_C));
                    mvwprintw(map_win, i, j,"%c",'A');
                    wattroff(map_win, COLOR_PAIR(COLOR_C));
                }break;

                case '*':{
                    wattron(map_win, COLOR_PAIR(COLOR_B));
                    mvwprintw(map_win, i, j,"%c", map->map[i][j]);
                    wattroff(map_win, COLOR_PAIR(COLOR_B));
                }break;

                case 't':case 'c':case'T':case'D':{
                    wattron(map_win, COLOR_PAIR(COLOR_M));
                    mvwprintw(map_win, i, j,"%c", map->map[i][j]);
                    wattroff(map_win, COLOR_PAIR(COLOR_M));
                }break;

                case '1':case '2':case '3':case '4':{
                    wattron(map_win, COLOR_PAIR(COLOR_P));
                    mvwprintw(map_win, i, j,"%c", map->map[i][j]);
                    wattroff(map_win, COLOR_PAIR(COLOR_P));
                }break;

                default:mvwprintw(map_win, i, j, "%c", map->map[i][j]);
            }
        }
    }
    box(map_win,0,0);
    refresh();
    wrefresh(map_win);
}
void print_server_info (){

    struct server_t*info = &server;
    WINDOW * info_win = newwin(21,90,1,60);

    mvwprintw(info_win,1,2,"%s :%d","Server PID",info->serverPID);
    mvwprintw(info_win,2,2,"%s :%d/%d","Camp X/Y",info->camp_x,info->camp_y);
    mvwprintw(info_win,3,2,"%s :%d","Round number",info->round_number);
    mvwprintw(info_win,5,2,"%s","Information about players:");


    for(int i=0;i<4;i++){
        if(info->player[i].type==NULL){
            mvwprintw(info_win,7,(i*22)+3,"%s %d","Player",i+1);
            mvwprintw(info_win,8,(i*22)+3,"%s :-","Player_PID");
            mvwprintw(info_win,9,(i*22)+3,"%s :-","Type");
            mvwprintw(info_win,10,(i*22)+3,"%s :-/-","Current X/Y");
            mvwprintw(info_win,11,(i*22)+3,"%s :-","Deaths");
        }
        else{
            mvwprintw(info_win,7,(i*22)+3,"%s %d","Player",i+1);
            mvwprintw(info_win,8,(i*22)+3,"%s :%d","Player_PID",info->player[i].playerPID);
            mvwprintw(info_win,9,(i*22)+3,"%s :%s","Type",info->player[i].type);
            mvwprintw(info_win,10,(i*22)+3,"%s :%d/%d","Current X/Y",info->player[i].x,info->player[i].y);
            mvwprintw(info_win,11,(i*22)+3,"%s :%d","Deaths",info->player[i].deaths);
        }
    }
    mvwprintw(info_win,13,2,"%s","Coins:");
    for(int i=0;i<info->number_of_players;i++){

        mvwprintw(info_win,14,(i*22)+3,"%s :%d","Carried",info->player[i].carried);
        mvwprintw(info_win,15,(i*22)+3,"%s :%d","Brought",info->player[i].brought);
    }
    mvwprintw(info_win,17,2,"%s%d","Number of beasts :",server.count_beast);
    if(server.count_beast == 15){
        mvwprintw(info_win,18,3,"%s","MAXIMUM NUMBER OF BEASTS");
    }
    mvwprintw(info_win,17,35,"%s%d","Number of treasures :",server.treasures_on_map);
    if(server.treasures_on_map== 100){
        mvwprintw(info_win,18,36,"%s","MAXIMUM NUMBER OF TREASURES");
    }
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
void * server_actions(){
    while(1) {
        server_action = getch();
        if(server_action == 'q' || server_action == 'Q')break;
    }
    pthread_exit(NULL);
}
void add_coin (char type){
    while(1){
        int x_coin = rand()%server.map->size_x;
        int y_coin = rand()%server.map->size_y;

        if(server.map->map[y_coin][x_coin] == ' '){
            server.map->map[y_coin][x_coin] = type;
            server.treasures_on_map++;
            break;
        }
    }
}
void add_beast(){
    while(1){
        int x_beast = rand()%server.map->size_x;
        int y_beast = rand()%server.map->size_y;

        if(server.map->map[y_beast][x_beast] == ' ') {
            server.map->map[y_beast][x_beast] = '*';

            server.count_beast++;
            pthread_mutex_init(&server.beasts[server.count_beast-1].make_move,NULL);
            pthread_mutex_init(&server.beasts[server.count_beast-1].read_move,NULL);
            server.beasts[server.count_beast-1].x = x_beast;
            server.beasts[server.count_beast-1].y = y_beast;
            server.beasts[server.count_beast-1].last_block = ' ';
            break;
        }

    }
    int * arg = calloc(1, sizeof(int));
    *arg = server.count_beast-1;
    pthread_create(&server.beast[server.count_beast-1],NULL,beast_move,(void*)arg);
}
void* beast_move (void *number) {
    int num = *(int*)number;
    free(number);
    while(1){
        pthread_mutex_lock(&server.beasts[num].make_move);
        server.beasts[num].move = rand()%4;
        pthread_mutex_unlock(&server.beasts[num].read_move);
        if(server_action=='q' || server_action == 'Q')break;
    }

    return NULL;
}
int can_go(int x,int y,enum move dir){

    switch (dir) {
        case UP:{
           if(server.map->map[y-1][x] != '_')return 1;
        }break;
        case DOWN:{
            if(server.map->map[y+1][x] != '_')return 1;
        }break;
        case RIGHT:{
            if(server.map->map[y][x+1] != '_')return 1;
        }break;
        case LEFT:{
            if(server.map->map[y][x-1] != '_')return 1;
        }break;
    }
    return 0;
}
void start_summon(){
    for(int i=0;i<2;i++){
        add_beast();
        add_coin('T');
    }
    for(int i=0;i<5;i++){
        add_coin('t');
    }
    for(int i=0;i<8;i++){
        add_coin('c');
    }
}
void* try_to_connect(){
    while(1) {
        sleep(1);
        if(server.number_of_players == 4){
            int fd = open("/home/wsj/Pulpit/siema/connect", O_WRONLY);
            write(fd, &server.number_of_players , sizeof(int));
            close(fd);
            continue;
        }
        int free_slot = -1;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (server.player[i].is_connected == 0) {
                free_slot = i;
                server.player[i].is_connected=1;
                break;
            }
        }
        if (free_slot == -1)
            continue;
        int fd = open("/home/wsj/Pulpit/siema/connect", O_WRONLY);
        write(fd, &free_slot, sizeof(int));
        close(fd);

        int *ar = calloc(1, sizeof(int));
        *ar = free_slot;
        pthread_create(&communication[free_slot], NULL,main_communication,(void*)ar);

        pthread_detach(communication[free_slot]);

        if(server.end)return NULL;
    }
}
void add_player(int slot){
    while(1){
        int x_player = rand()%server.map->size_x;
        int y_player = rand()%server.map->size_y;

        if(server.map->map[y_player][x_player] == ' ') {
            server.map->map[y_player][x_player] = (char)(slot+49);

            server.number_of_players++;
            server.player[slot].x = x_player;
            server.player[slot].y = y_player;
            server.player[slot].last_block = ' ';
            break;
        }

    }
}
void clear_map(){

    for(int i=0;i<server.map->size_x;i++){
        for(int j=0;j<server.map->size_y;j++){
            switch(server.map->map[j][i]){
                case '1':{
                    if(j != server.player[0].y || i != server.player[0].x){
                        server.map->map[j][i] = server.player[0].last_block;
                    }
                }break;
                case '2':{
                    if(j != server.player[1].y || i != server.player[1].x){
                        server.map->map[j][i] = server.player[1].last_block;
                    }
                }break;
                case '3':{
                    if(j != server.player[2].y || i != server.player[2].x){
                        server.map->map[j][i] = server.player[2].last_block;
                    }
                }break;
                case '4':{
                    if(j != server.player[3].y || i != server.player[3].x){
                        server.map->map[j][i] = server.player[3].last_block;
                    }
                }break;
                default:break;
            }

        }
    }

}
void beast_call(struct beast_t* beast){
    if((server.map->map[beast->y-1][beast->x] >='1' && server.map->map[beast->y-1][beast->x] <='4')||(server.map->map[beast->y-2][beast->x] >='1' && server.map->map[beast->y-2][beast->x] <='4')){
        beast->move = UP;
        return;
    }
    else if((server.map->map[beast->y+1][beast->x] >='1' && server.map->map[beast->y+1][beast->x] <='4')||(server.map->map[beast->y+2][beast->x] >='1' && server.map->map[beast->y+2][beast->x] <='4')){
        beast->move = DOWN;
        return;
    }
    else if((server.map->map[beast->y][beast->x-1] >='1' && server.map->map[beast->y][beast->x-1] <='4')||(server.map->map[beast->y][beast->x-2] >='1' && server.map->map[beast->y][beast->x-2] <='4')){
        if(can_go(beast->x,beast->y,UP)==1)
            beast->move = LEFT;
        return;
    }
    else if((server.map->map[beast->y][beast->x+1] >='1' && server.map->map[beast->y][beast->x+1] <='4')||(server.map->map[beast->y][beast->x+2] >='1' && server.map->map[beast->y][beast->x+2] <='4')){
        beast->move = RIGHT;
        return;
    }
    else if(server.map->map[beast->y-1][beast->x-1] != '_'){
        if(server.map->map[beast->y-2][beast->x-1] >='1' && server.map->map[beast->y-2][beast->x-1] <='4'){
            if(can_go(beast->x,beast->y,UP)==1)
                beast->move = UP;
            return;
        }
        else if(server.map->map[beast->y-1][beast->x-2] >='1' && server.map->map[beast->y-1][beast->x-2] <='4'){
            if(can_go(beast->x,beast->y,LEFT)==1)
                beast->move = LEFT;
            return;
        }
    }
    else if(server.map->map[beast->y-1][beast->x+1] != '_'){
        if(server.map->map[beast->y-2][beast->x+1] >='1' && server.map->map[beast->y-2][beast->x+1] <='4'){
            if(can_go(beast->x,beast->y,UP)==1)
                beast->move = UP;
            return;
        }
        else if(server.map->map[beast->y-1][beast->x+2] >='1' && server.map->map[beast->y-1][beast->x+2] <='4'){
            if(can_go(beast->x,beast->y,RIGHT)==1)
                beast->move = RIGHT;
            return;
        }
    }
    else if(server.map->map[beast->y+1][beast->x-1] != '_'){
        if(server.map->map[beast->y+2][beast->x-1] >='1' && server.map->map[beast->y+2][beast->x-1] <='4'){
            if(can_go(beast->x,beast->y,DOWN)==1)
                beast->move = DOWN;
            return;
        }
        else if(server.map->map[beast->y+1][beast->x-2] >='1' && server.map->map[beast->y+1][beast->x-2] <='4'){
            if(can_go(beast->x,beast->y,LEFT)==1)
                beast->move = LEFT;
            return;
        }
    }
    else if(server.map->map[beast->y+1][beast->x+1] != '_'){
        if(server.map->map[beast->y+2][beast->x+1] >='1' && server.map->map[beast->y+2][beast->x+1] <='4'){
            if(can_go(beast->x,beast->y,DOWN)==1)
                beast->move = DOWN;
            return;
        }
        else if(server.map->map[beast->y+1][beast->x+2] >='1' && server.map->map[beast->y+1][beast->x+2] <='4'){
            if(can_go(beast->x,beast->y,RIGHT)==1)
                beast->move = RIGHT;
            return;
        }
    }
}

int treasures(int slot,int y,int x){
    if(server.map->map[y][x] == 'c'){
        server.player[slot].carried += coin;
        return 1;
    }
    else if(server.map->map[y][x] == 't'){
        server.player[slot].carried += treasure;
        return 1;
    }
    else if(server.map->map[y][x] == 'T'){
        server.player[slot].carried += big_treasure;
        return 1;
    }
    else if(server.map->map[y][x] == 'D') {
        for (int i = 0; i < server.count_dropped_treasures; i++) {
            if (y == server.dropped[i].y && x == server.dropped[i].x && server.dropped[i].is_picked_up == 0) {
                server.player[slot].carried += server.dropped[i].amount;
                return 1;
            }
        }
    }
    else if(server.map->map[y][x] == 'O'){
        server.player[slot].brought += server.player[slot].carried;
        server.player[slot].carried = 0;
    }
    else if(server.map->map[y][x] == '#'){
        server.player[slot].in_bush=1;
    }
    return 0;
}
void add_dropped_treasure(int amount,int x,int y,char last_block){

    if(server.count_dropped_treasures==0)
        server.dropped = calloc(1,sizeof(struct droped_treasure_t));
    else{
        server.dropped = realloc(server.dropped,(server.count_dropped_treasures+1)* sizeof(struct droped_treasure_t));
    }
    server.dropped[server.count_dropped_treasures].x = x;
    server.dropped[server.count_dropped_treasures].y = y;
    server.dropped[server.count_dropped_treasures].amount = amount;
    server.dropped[server.count_dropped_treasures].last_block = last_block;
    server.dropped[server.count_dropped_treasures].is_picked_up = 0;
    server.count_dropped_treasures++;
}
void* main_communication(void* arg){
    int slot = *(int*)arg;
    free(arg);
    char fifo_to_player[40];
    sprintf(fifo_to_player, "/home/wsj/Pulpit/siema/to_player%d", slot);
    char fifo_from_player[40];
    sprintf(fifo_from_player, "/home/wsj/Pulpit/siema/from_player%d", slot);

    mkfifo(fifo_to_player,777);
    mkfifo(fifo_from_player,777);

    int to_player = open(fifo_to_player,O_WRONLY);
    int from_player = open(fifo_from_player,O_RDONLY);

    server.number_of_players++;

    add_player(slot);
    server.player[slot].type = strdup("HUMAN");

    while(1){
        pthread_mutex_lock(&server.player[slot].make_move);
        for(int i=0;i<5;i++){
            for(int j=0;j<5;j++){
                if(write(to_player,&server.map->map[server.player[slot].y-2+i][server.player[slot].x-2+j], sizeof(char)) !=
                        sizeof(char))break;
            }

        }
        if(write(to_player,&server.serverPID, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.camp_x, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.camp_y, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.round_number, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.player[slot].x, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.player[slot].y, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.player[slot].deaths, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.player[slot].carried, sizeof(int)) != sizeof(int))
            break;
        if(write(to_player,&server.player[slot].brought, sizeof(int)) != sizeof(int))
            break;

        if(read(from_player,&server.player[slot].move_player, sizeof(int))!= sizeof(int))
            break;
        if(read(from_player,&server.player[slot].playerPID, sizeof(int))!= sizeof(int))
            break;
        if(read(from_player,&server.player[slot].number, sizeof(int))!= sizeof(int))
            break;

        if(server.player[slot].in_bush == 1){
            server.player[slot].in_bush = 0;
            continue;
        }
        if(can_go(server.player[slot].x,server.player[slot].y,server.player[slot].move_player)==1){

            server.map->map[server.player[slot].y][server.player[slot].x] = server.player[slot].last_block;
            int tmp=0;
            switch (server.player[slot].move_player) {
                case UP:{
                    tmp = treasures(slot,server.player[slot].y-1,server.player[slot].x);
                    server.player[slot].y--;
                }break;
                case DOWN:{
                    tmp = treasures(slot,server.player[slot].y+1,server.player[slot].x);
                    server.player[slot].y++;
                }break;
                case LEFT:{
                    tmp = treasures(slot,server.player[slot].y,server.player[slot].x-1);
                    server.player[slot].x--;
                }break;
                case RIGHT:{
                    tmp = treasures(slot,server.player[slot].y,server.player[slot].x+1);
                    server.player[slot].x++;
                }break;
            }
            if(tmp == 1)
                server.player[slot].last_block = ' ';
            else
                server.player[slot].last_block = server.map->map[server.player[slot].y][server.player[slot].x];

            server.map->map[server.player[slot].y][server.player[slot].x] = (char)(slot+'0'+1);
        }
        for(int i=0;i<server.count_beast;i++){
            if((server.player[slot].x == server.beasts[i].x) && (server.player[slot].y == server.beasts[i].y)){
                server.player[slot].deaths++;

                if(server.beasts[i].last_block >='1' && server.beasts[i].last_block <='4'){
                    server.beasts[i].last_block = server.player[slot].last_block;
                }

                if(server.player[slot].carried > 0) {
                    add_dropped_treasure(server.player[slot].carried, server.player[slot].x, server.player[slot].y,server.beasts[i].last_block);

                    server.beasts[i].last_block = 'D';
                    server.player[slot].carried = 0;
                }

                add_player(slot);
                break;
            }
        }
        clear_map();
    }
    server.map->map[server.player[slot].y][server.player[slot].x]=server.player[slot].last_block;

    server.player[slot].x=0;
    server.player[slot].y=0;
    free(server.player[slot].type);
    server.player[slot].type =NULL;
    server.player[slot].brought=0;
    server.player[slot].deaths=0;
    server.player[slot].carried=0;
    server.player[slot].number=0;
    server.player[slot].is_connected=0;
    server.player[slot].in_bush=0;
    server.number_of_players--;

    close(to_player);
    close(from_player);
    remove(fifo_to_player);
    remove(fifo_from_player);

    return NULL;
}

int main_server (struct map_t * map){
    if(map == NULL){
        return -1;
    }
    server.map = map;
    server.serverPID = getpid();
    server.camp_x = 26;
    server.camp_y = 14;
    server.round_number = 0;
    server.count_dropped_treasures = 0;
    server.treasures_on_map=0;
    server.count_beast = 0;
    server.number_of_players = 0;

    initscr();
    pthread_t input;

    pthread_t player_thread;
    mkfifo("/home/wsj/Pulpit/siema/connect",777);
    for(int i=0;i<4;i++){
        server.player[i].is_connected=0;
        server.player[i].deaths=0;
        server.player[i].carried=0;
        server.player[i].brought=0;
        server.player[i].in_bush=0;
    }

    pthread_create(&player_thread,NULL,try_to_connect,NULL);
    pthread_create(&input,NULL,server_actions,NULL);



    while(1){
        if(server.round_number == 0 ){
            start_summon();
        }
        print_map(map);
        print_server_info();

        if(server_action == 'q' || server_action == 'Q')break;

        for(int i=0;i<4;i++){
            pthread_mutex_unlock(&server.player[i].make_move);
        }


        switch ((char)server_action) {
            case 'b':case 'B':{
                if(server.count_beast >= 15)break;
                add_beast();
            }break;
            case'c':{
                add_coin('c');
            }break;
            case't':{
                add_coin('t');
            }break;
            case'T':{
                add_coin('T');
            }break;
        }
        server_action = '\0';
        usleep(600*1000);

        for(int i=0;i<server.count_beast;i++) {
            pthread_mutex_unlock(&server.beasts[i].make_move);
        }
        for(int i=0;i<server.count_beast;i++){
            pthread_mutex_lock(&server.beasts[i].read_move);
            beast_call(&server.beasts[i]);
            if(can_go(server.beasts[i].x,server.beasts[i].y,server.beasts[i].move)==1){


                if(server.beasts[i].last_block == '*')
                    server.beasts[i].last_block = ' ';

                server.map->map[server.beasts[i].y][server.beasts[i].x] = server.beasts[i].last_block;

                switch (server.beasts[i].move) {
                    case UP:{
                        server.beasts[i].y--;
                    }break;
                    case DOWN:{
                        server.beasts[i].y++;
                    }break;
                    case LEFT:{
                        server.beasts[i].x--;
                    }break;
                    case RIGHT:{
                        server.beasts[i].x++;
                    }break;
                }
                server.beasts[i].last_block = server.map->map[server.beasts[i].y][server.beasts[i].x];
                server.map->map[server.beasts[i].y][server.beasts[i].x] = '*';
                if(server.beasts[i].last_block >='1' && server.beasts[i].last_block <='4'){
                    server.beasts[i].last_block = ' ';
                }
            }
        }
        server.round_number++;
    }
    endwin();
    pthread_join(input,NULL);
    pthread_join(player_thread,NULL);
    free_all();
    return 0;
}
void free_all(){
    for(int i=0;i<server.map->size_y;i++){
        free(server.map->map[i]);
    }
    for(int i=0;i<server.count_beast;i++){
        pthread_mutex_destroy(&server.beasts[i].make_move);
        pthread_mutex_destroy(&server.beasts[i].read_move);
    }
    free(server.map->map);
    free(server.dropped);
    free(server.map);
}








