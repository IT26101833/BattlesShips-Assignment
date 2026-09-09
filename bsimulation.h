#ifndef BSIMULATION_H
#define BSIMULATION_H

#include <stdio.h>
#include "structures.h"

void writeInitialConditions(FILE *logFile, Battleship *b, EscortShip escorts[], int numEscorts);
void logAttackOrder(FILE *logFile, Battleship b, EscortShip escorts[], int numEscorts);
void moveBattleshipToRandomPoint(Battleship *b, double gridSize);
int selectTargetEscort(Battleship b, EscortShip escorts[], int numEscorts);
void runfullSimulation(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile);

#endif