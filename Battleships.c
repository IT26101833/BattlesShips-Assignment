#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structures.h"
#include "ui"

int main() {
    SimConfig config = {1000.0, (unsigned int)time(NULL), 10};
    srand(config.seed);

    Battleship b;
    
    return 0;
}