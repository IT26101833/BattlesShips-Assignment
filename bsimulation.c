#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <physics.h>
#include "structures.h"

#define GRAVITY 10
#define PI 3.14159

double getDistance(Position p1, Position p2);
bool canHitTarget(Position shooter, Position target, double vMin, double vMax, double aMinDeg, double aMaxDeg, double *timeOfFlight);
double getDegradedImpact(double baseImpact, double gamma, int shotsFired);


int selectTargetEscort(Battleship b, EscortShip escorts[], int numEscorts) {
    int targetIdx = -1;
    double maxImpact = -1.0;

    for (int i = 0; i < numEscorts; i++) {
        if (escorts[i].destroyed) continue;

        double tof;
        bool bCanHitE = canHitTarget(b.pos, escorts[i].pos, b.vMin, b.vMax, b.angleMin, b.angleMax, &tof);
        if (!bCanHitE) continue;

        bool eCanHitB = canHitTarget(escorts[i].pos, b.pos, escorts[i].vMin, escorts[i].vMax, 
                                     escorts[i].angleMin, escorts[i].angleMax, NULL);

        //calculating current mpact of escortship
        double currentImpact = getDegradedImpact(escorts[i].impactPower, escorts[i].gamma, escorts[i].shotsFired);

        
        if (eCanHitB) currentImpact += 10.0;

        if (currentImpact > maxImpact) {
            maxImpact = currentImpact;
            targetIdx = i;
        }
    }
    return targetIdx;
}

//function for log file printinga and siulation monitoring
void runfullSimulation(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile) {
    double currentTime = 0.0;
    double bNextShotTime = 0.0;
    int escortsRemaining = numEscorts;

    if (logFile) fprintf(logFile, "--- SIMULATION START ---\n");

    while (!b->destroyed && escortsRemaining > 0 && currentTime < 1000.0) {
        //battleship firing logic
        if (currentTime >= bNextShotTime) {
            int targetIdx = selectTargetEscort(*b, escorts, numEscorts);
            if (targetIdx != -1) {
                b->shotsFired++;
                double currentBImpact = getDegradedImpact(b->impactPower, b->gamma, b->shotsFired);
                
                //attack methods damege calculatipn
                escorts[targetIdx].destroyed = true;
                escortsRemaining--;

                if (logFile) {
                    fprintf(logFile, "[t=%.2fs] Battleship fired shot #%d at Escort E%d (Destroyed! Impact=%.3f)\n",
                            currentTime, b->shotsFired, escorts[targetIdx].id, currentBImpact);
                }
            }
            bNextShotTime = currentTime + b->reloadTime;
        }

        //escort ships firing 
        for (int i = 0; i < numEscorts; i++) {
            if (escorts[i].destroyed) continue;

            if (currentTime >= escorts[i].nextFiringTime) {
                double tof;
                if (canHitTarget(escorts[i].pos, b->pos, escorts[i].vMin, escorts[i].vMax, 
                                 escorts[i].angleMin, escorts[i].angleMax, &tof)) {
                    
                    escorts[i].shotsFired++;
                    double currentEImpact = getDegradedImpact(escorts[i].impactPower, escorts[i].gamma, escorts[i].shotsFired);
                    b->health -= currentEImpact;

                    if (logFile) {
                        fprintf(logFile, "[t=%.2fs] Escort E%d fired shot #%d at Battleship (Hit! Damage=%.2f%%, B-Health=%.2f%%)\n",
                                currentTime, escorts[i].id, escorts[i].shotsFired, currentEImpact * 100.0, b->health * 100.0);
                    }

                    if (b->health <= 0.0) {
                        b->destroyed = true;
                        if (logFile) fprintf(logFile, "[t=%.2fs] BATTLESHIP DESTROYED!\n", currentTime);
                        break;
                    }
                }
                escorts[i].nextFiringTime = currentTime + escorts[i].reloadTime;
            }
        }

       
        currentTime += 0.5;
    }

    if (logFile) {
        fprintf(logFile, "--- SIMULATION END ---\nFinal Battleship Health: %.2f%%\nEscorts Destroyed: %d/%d\nDuration: %.2fs\n",
                b->health > 0 ? b->health * 100.0 : 0.0, numEscorts - escortsRemaining, numEscorts, currentTime);
    }
}