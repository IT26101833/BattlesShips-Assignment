#include <stdio.h>
#include <string.h>
#include <time.h>
#include "structures.h"
#include "ui.h"
#include <stdlib.h>
#include <stdbool.h>


int main(){



    SimConfig config = {1000.0, (unsigned int)time(NULL), 10};
    srand(config.seed);


    //initializing a battleship
    Battleship b;
    strcpy(b.typeName, "USS Iowa BB-61");
    strcpy(b.typeNotation, "U");
    
    b.pos.x = 500.0;
    b.pos.y = 500.0;
    b.vMin = 40.0;
    b.vMax = 120.0;
    b.angleMin = 5.0;
    b.angleMax = 85.0;
    b.impactPower = 0.5;
    b.gamma = 0.02;
    b.shotsFired = 0;
    b.health = 1.0;
    b.destroyed = false;
    b.reloadTime = 3.0;

    main_menu(&config, &b);
    
    return 0;
}