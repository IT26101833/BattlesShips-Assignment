#ifndef PART1_H
#define PART1_H

#include <stdio.h>
#include <stdbool.h>
#include "structures.h"

int runPart1A(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile, bool escortsDealPartialDamage, int *destroyerOut);
void runPart1B(Battleship *b, EscortShip escorts[], int numEscorts, int k, double jamAfterIterations, double jamAngleDeg, const char *baseFile, double gridSize, bool escortsDealPartialDamage);

#endif