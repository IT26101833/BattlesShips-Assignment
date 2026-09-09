#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "physics.h"
#include "part1.h"
#include "structures.h"

//core battle for Part 1-A and Part 1-C.
int runPart1A(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile, bool escortsDealPartialDamage, int *destroyerOut){
    int escortsRemaining = numEscorts;
    //store the id of the ship that destroyed the target
    int destroyerId = -1;
    double shotTime = 0.0;

    if (destroyerOut) *destroyerOut = -1;

    //battleship fires at every escort it can hit, a single hit destroys an E ship
    for(int i = 0; i < numEscorts; i++){
        if (escorts[i].destroyed) continue;

        double tof;
        bool bCanHitE = canHitTarget(b->pos, escorts[i].pos, b->vMin, b->vMax, b->angleMin, b->angleMax, &tof);
        if (!bCanHitE) continue;

        b->shotsFired++;
        shotTime += tof; //time of flight is the time to hit this escort
        escorts[i].destroyed = true;
        escortsRemaining--;

        if (logFile){
            fprintf(logFile, "[t=%.2fs] Battleship hit Escort E%d with a single shot "
                             "(Index=%d, Time-to-hit=%.2fs).\n", shotTime, escorts[i].id, escorts[i].id, tof);

        }
    }

    //surviving escorts attack the battleship
    if (escortsRemaining > 0 && !b->destroyed) {
        for(int i = 0; i < numEscorts; i++) {
            if (escorts[i].destroyed) continue;

            double tof;
            bool eCanHitB = canHitTarget(escorts[i].pos, b->pos, escorts[i].vMin, escorts[i].vMax, escorts[i].angleMin, escorts[i].angleMax, &tof);
            
            if (!eCanHitB) continue;

            escorts[i].shotsFired++;
            shotTime += tof;

            if (escortsDealPartialDamage) {

                b->health -= escorts[i].impactPower;
                if (logFile) {
                    fprintf(logFile, "[t=%.2fs] Escort E%d hit the Battleship "
                                     "(Impact=%.3f, B-Health=%.2f%%).\n",
                            shotTime, escorts[i].id, escorts[i].impactPower,
                            b->health > 0 ? b->health * 100.0 : 0.0);
                }
                if (b->health <= 0.0) {
                    b->health = 0.0;
                    b->destroyed = true;
                    destroyerId = escorts[i].id;
                    if (logFile) {

                        fprintf(logFile, "[t=%.2fs] BATTLESHIP DESTROYED by cumulative impact.\n", shotTime);
                    }
                    break;
                }
            } else {
                //a single hit destroys B
                b->health = 0.0;
                b->destroyed = true;
                destroyerId = escorts[i].id;
                if (logFile) {
                    fprintf(logFile, "[t=%.2fs] Escort E%d hit the Battleship (single shot). "
                                     "B is DESTROYED.\n", shotTime, escorts[i].id);
                }
                break;
            }
        }
    }

    if (logFile) {
        fprintf(logFile, "--- PART 1 RESULT ---\n");
        if(b->destroyed) {

            fprintf(logFile, "Battleship sank. Sunk by Escort E%d. Duration=%.2fs.\n",
                    destroyerId, shotTime);
        } else{
            fprintf(logFile, "Battleship survived. Escorts destroyed: %d/%d. Duration=%.2fs.\n", numEscorts - escortsRemaining, numEscorts, shotTime);
        }
    }

    if (destroyerOut) *destroyerOut = destroyerId;

    return b->destroyed ? 1 : 0;
}

//battleship moves along k randomly generated points inside the canvas.

void runPart1B(Battleship *b, EscortShip escorts[], int numEscorts, int k, double jamAfterIterations, double jamAngleDeg, const char *baseFile,
               double gridSize, bool escortsDealPartialDamage){
    int escortsRemaining = numEscorts;

    //k point where the target sank
    int sankAtPoint = -1;
    int destroyerId = -1;

    for (int iter = 0; iter < k; iter++) {
        //pick a random point inside the canvas for this iteration
        double px = (double)(rand() % ((int)gridSize + 1));

        double py = (double)(rand() % ((int)gridSize + 1));

        b->pos.x = px;
        b->pos.y = py;

        //apply gun jamming after t iterations 
        if (jamAngleDeg > 0.0 && iter >= jamAfterIterations){
            b->angleMin = jamAngleDeg;
            b->angleMax = 90.0;
        }

        char fname[256];
        snprintf(fname, sizeof(fname), "%s_part1B_iter%d.txt", baseFile, iter + 1);
        FILE *logFile = fopen(fname, "w");
        if (!logFile){

            printf("Failed to open %s for logging.\n", fname);
            return;
        }

        fprintf(logFile, "--- PART 1-B (iteration %d of %d) ---\n", iter + 1, k);
        fprintf(logFile, "Battleship at (%.1f, %.1f), jam angle = %.1f\n",
                b->pos.x, b->pos.y, jamAngleDeg);

        //save initial battlefield conditions for this iteration
        fprintf(logFile, "--- INITIAL CONDITIONS ---\n");
        fprintf(logFile, "Battleship: %s (%s) Vmax=%.1f Vmin=%.1f angles %.1f-%.1f\n", b->typeName, b->typeNotation, b->vMax, b->vMin, b->angleMin, b->angleMax);
        for(int i = 0; i < numEscorts; i++){
            if (escorts[i].destroyed) continue;
            fprintf(logFile, "Escort E%d (%s) at (%.1f, %.1f): Vmin=%.1f Vmax=%.1f "
                             "angles %.1f-%.1f type=%s\n", escorts[i].id, escorts[i].typeNotation, escorts[i].pos.x,
                    escorts[i].pos.y, escorts[i].vMin, escorts[i].vMax,
                    escorts[i].angleMin, escorts[i].angleMax, escorts[i].typeName);
        }

        int result = runPart1A(b, escorts, numEscorts, logFile,
                               escortsDealPartialDamage, &destroyerId);

        //recount how many escorts are still alive
        escortsRemaining = 0;
        for (int i = 0; i < numEscorts; i++) {
            if (!escorts[i].destroyed) escortsRemaining++;
        }

        fclose(logFile);

        if (result) {
            
            sankAtPoint = iter + 1;
            printf("B sank at iteration %d (point %d) by Escort E%d. Simulation 1-B stopped.\n", iter + 1, iter + 1, destroyerId);
            break;
        }
    }

    if (sankAtPoint == -1){

        printf("Part 1-B: Battleship survived all %d points. Escorts remaining: %d.\n", k, escortsRemaining);
    } else {
        printf("Part 1-B: Battleship sank at point %d of %d.\n", sankAtPoint, k);
    }
}