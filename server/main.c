#include <stdio.h>
#include <signal.h>
#include "server.h"
int main() {

    signal(SIGPIPE, SIG_IGN);

    srand(time(NULL));

    struct  map_t* map = read_map("map.txt");
    main_server(map);

    return 0;
}
