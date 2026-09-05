#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "physics.h"
#include "structures.h"

#define GRAVITY 10
#define PI 3.14159


//function th run part 1 A
int runPart1A(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile){
    int escortsRemaining = numEscorts;
    int destroyerId = -1;
    double shotTime = 0.0;

  

    for (int i = 0; i < numEscorts; i++) {
        if (escorts[i].destroyed) continue;

        double tof;
        bool bCanHitE = canHitTarget(b->pos, escorts[i].pos, b->vMin, b->vMax,b->angleMin, b->angleMax, &tof);
        if (!bCanHitE) continue;

        
        b->shotsFired++;
        shotTime += 1.0; //one time unit per effective shot for the time-to-hit record
        escorts[i].destroyed = true;
        escortsRemaining--;

        if (logFile) {
            fprintf(logFile, "[t=%.1fs] Battleship hit Escort E%d with a single shot (Index=%d, Time-to-hit=%.1fs).\n", shotTime, escorts[i].id, escorts[i].id, shotTime);
        }
    }

    
    if (escortsRemaining > 0 && !b->destroyed) {
        for (int i = 0; i < numEscorts; i++) {
            if (escorts[i].destroyed) continue;

            bool eCanHitB = canHitTarget(escorts[i].pos, b->pos, escorts[i].vMin, escorts[i].vMax, escorts[i].angleMin, escorts[i].angleMax, NULL);
            if (eCanHitB) {
                escorts[i].shotsFired++;
                b->health = 0.0;
                b->destroyed = true;
                destroyerId = escorts[i].id;
                shotTime += 1.0;
                if (logFile){
                    fprintf(logFile, "[t=%.1fs] Escort E%d hit the Battleship (single shot). B is DESTROYED.\n",
                            shotTime, escorts[i].id);
                }
                break;
            }
        }
    }

    if (logFile) {
        fprintf(logFile, "--- PART 1-A RESULT ---\n");
        if (b->destroyed) {
            fprintf(logFile, "Battleship sank. Sunk by Escort E%d. Duration=%.1fs.\n", destroyerId, shotTime);
        } else {
            fprintf(logFile, "Battleship survived. Escorts destroyed: %d/%d. Duration=%.1fs.\n",numEscorts - escortsRemaining, numEscorts, shotTime);
        }
    }

    if (b->destroyed) {
        printf("Part 1-A: Battleship was DESTROYED by Escort E%d after %.1fs.\n", destroyerId, shotTime);
    } else {
        printf("Part 1-A: Battleship SURVIVED. Escorts destroyed: %d/%d after %.1fs.\n", numEscorts - escortsRemaining, numEscorts, shotTime);
    }

    return b->destroyed ? 1 : 0;
}



int selectTargetEscort(Battleship b, EscortShip escorts[], int numEscorts){

    int targetIdx = -1;
    double maxImpact = -1.0;

    for (int i = 0; i < numEscorts; i++) {
        if (escorts[i].destroyed) continue;

        double tof;
        bool bCanHitE = canHitTarget(b.pos, escorts[i].pos, b.vMin, b.vMax, b.angleMin, b.angleMax, &tof);
        if (!bCanHitE) continue;

        bool eCanHitB = canHitTarget(escorts[i].pos, b.pos, escorts[i].vMin, escorts[i].vMax, escorts[i].angleMin, escorts[i].angleMax, NULL);

        //calculating current impact of escortship
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
    int destroyerId = -1;

    //save initial battlefield conditions to the log file
    if (logFile){
        fprintf(logFile, "--- INITIAL CONDITIONS ---\n");
        fprintf(logFile, "Battleship: %s (%s) at (%.1f, %.1f)\n", b->typeName, b->typeNotation, b->pos.x, b->pos.y);
        fprintf(logFile, "  Vmin=%.1f Vmax=%.1f, angles %.1f-%.1f, impact=%.3f, gamma=%.3f, reload=%.1fs\n",
                b->vMin, b->vMax, b->angleMin, b->angleMax, b->impactPower, b->gamma, b->reloadTime);



        for (int i = 0; i < numEscorts; i++) {
            fprintf(logFile, "Escort E%d (%s) at (%.1f, %.1f): Vmin=%.1f Vmax=%.1f, angles %.1f-%.1f, impact=%.3f, reload=%.1fs\n",
                    escorts[i].id, escorts[i].typeNotation, escorts[i].pos.x, escorts[i].pos.y,
                    escorts[i].vMin, escorts[i].vMax, escorts[i].angleMin, escorts[i].angleMax,
                    escorts[i].impactPower, escorts[i].reloadTime);
        }
        fprintf(logFile, "--- SIMULATION START ---\n");
    }

    while (!b->destroyed && escortsRemaining > 0 && currentTime < 1000.0) {
        //battleship firing logic
        if (currentTime >= bNextShotTime) {
            int targetIdx = selectTargetEscort(*b, escorts, numEscorts);
            if (targetIdx != -1) {
                b->shotsFired++;
                double currentBImpact = getDegradedImpact(b->impactPower, b->gamma, b->shotsFired);



                //damage the target- degradation means a ship may need several hits
                escorts[targetIdx].health -= currentBImpact;
                if (escorts[targetIdx].health <= 0.0) {
                    escorts[targetIdx].destroyed = true;
                    escortsRemaining--;
                    if (logFile) {
                        fprintf(logFile, "[t=%.2fs] Battleship fired shot #%d at Escort E%d (Destroyed! Impact=%.3f)\n",
                                currentTime, b->shotsFired, escorts[targetIdx].id, currentBImpact);
                    }
                } else if (logFile) {
                    fprintf(logFile, "[t=%.2fs] Battleship fired shot #%d at Escort E%d (Hit! Damage=%.3f, E%d-health=%.2f%%)\n",
                            currentTime, b->shotsFired, escorts[targetIdx].id, currentBImpact,
                            escorts[targetIdx].id, escorts[targetIdx].health * 100.0);
                }
            }
            bNextShotTime = currentTime + b->reloadTime;
        }

        //escort ships firing 
        for (int i = 0; i < numEscorts; i++){
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
                        destroyerId = escorts[i].id;
                        if (logFile) fprintf(logFile, "[t=%.2fs] BATTLESHIP DESTROYED!\n", currentTime);
                        break;
                    }
                }


                escorts[i].nextFiringTime = currentTime + escorts[i].reloadTime;
            }
        }

       
        currentTime += 0.5;
    }

    if (logFile){
        fprintf(logFile, "--- SIMULATION END ---\nFinal Battleship Health: %.2f%%\nEscorts Destroyed: %d/%d\nDuration: %.2fs\n",
                b->health > 0 ? b->health * 100.0 : 0.0, numEscorts - escortsRemaining, numEscorts, currentTime);
    }

    //display the result on the console too
    if (b->destroyed){

        printf("Battleship was DESTROYED by Escort E%d after %.2fs.\n", destroyerId, currentTime);
    }else {

        printf("Battleship SURVIVED (health %.2f%%). Escorts destroyed: %d/%d over %.2fs.\n",
               b->health * 100.0, numEscorts - escortsRemaining, numEscorts, currentTime);
    }
}