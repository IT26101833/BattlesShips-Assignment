#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structures.h"


int main() {
    SimConfig config = {1000.0, (unsigned int)time(NULL), 10};
    srand(config.seed);

    Battleship b;
    strcpy(b.typeName, "USS Iowa BB-61");
    strcpy(b.typeNotation, "U");
    b.pos.x = 500.0;
    b.pos.y = 500.0;
    b.vMin = 100.0;
    b.vMax = 400.0;
    b.angleMin = 0.0;
    b.angleMax = 90.0;
    b.impactPower = 0.5;
    b.gamma = 0.02;
    b.shotsFired = 0;
    b.health = 1.0;
    b.destroyed = false;
    b.reloadTime = 5.0;

    main_menu(&config, &b);
    
    return 0;
}