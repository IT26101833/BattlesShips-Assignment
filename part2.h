#ifndef PART2_H
#define PART2_H

#include <stdio.h>
#include <stdbool.h>
#include "structures.h"

void runPart2A(Battleship *b, EscortShip escorts[], int numEscorts, int k, double gridSize, bool escortsDealPartialDamage);
void runPart2B(Battleship *b, EscortShip escorts[], int numEscorts, int k, double gridSize, bool escortsDealPartialDamage);
void runPart2CPath(Battleship *b, EscortShip escorts[], int numEscorts, int k, double gridSize, const char *baseFile);

#endif