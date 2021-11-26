#include <stdio.h>
#include <stdlib.h>

#include "stats.h"


int main(int argc, char *argv[])
{
    int i = 0;
    stats_t stats;

    stats_init(&stats);
    for(i=0; i< 100; i+=2) {
        stats_add(&stats, i);
    }
    stats_print(&stats);

    return 0;
}

