#include <stdio.h>
#include <stdlib.h>
#include "physics.h"
#include "bsimulation.h"
#include "structures.h"

#define BSIM_MAX_ESCORTS 50

//write the initial battlefield conditions to a log file
void writeInitialConditions(FILE *logFile, Battleship *b, EscortShip escorts[], int numEscorts){
    if (!logFile) return;

    fprintf(logFile, "Battleship: %s (%s) at (%.1f, %.1f)\n",
            b->typeName, b->typeNotation, b->pos.x, b->pos.y);
    fprintf(logFile, "  Vmin=%.1f Vmax=%.1f, angles %.1f-%.1f, impact=%.3f, "
                     "gamma=%.3f, reload (T_B)=%.1fs\n",
            b->vMin, b->vMax, b->angleMin, b->angleMax, b->impactPower,
            b->gamma, b->reloadTime);

    for(int i = 0; i < numEscorts; i++){
        if (escorts[i].destroyed) continue;
        fprintf(logFile, "Escort E%d (%s) at (%.1f, %.1f): Vmin=%.1f Vmax=%.1f, "
                         "angles %.1f-%.1f, impact=%.3f, gamma=%.3f, reload (T_E)=%.1fs\n", escorts[i].id, escorts[i].typeNotation, escorts[i].pos.x,
                        escorts[i].pos.y, escorts[i].vMin, escorts[i].vMax, escorts[i].angleMin, escorts[i].angleMax, escorts[i].impactPower,
                escorts[i].gamma, escorts[i].reloadTime);

    }
}

//log the attack order within its attack range
void logAttackOrder(FILE *logFile, Battleship b, EscortShip escorts[], int numEscorts){
    if (!logFile) return;

    int orderIdx[BSIM_MAX_ESCORTS];
    double orderScore[BSIM_MAX_ESCORTS];
    int n = 0;

    for(int i = 0; i < numEscorts && n < BSIM_MAX_ESCORTS; i++){
        if (escorts[i].destroyed) continue;

        double tof;
        if (!canHitTarget(b.pos, escorts[i].pos, b.vMin, b.vMax,
                          b.angleMin, b.angleMax, &tof)) continue;

        bool eCanHitB = canHitTarget(escorts[i].pos, b.pos, escorts[i].vMin,
                                     escorts[i].vMax, escorts[i].angleMin,
                                     escorts[i].angleMax, NULL);

        double score = getDegradedImpact(escorts[i].impactPower, escorts[i].gamma,
                                         escorts[i].shotsFired);
        if (eCanHitB) score += 10.0;   //increaes threat level

        orderIdx[n] = i;
        orderScore[n] = score;
        n++;
    }

    //sort the reachable escortss by descending threat scores
    for (int a = 0; a < n - 1; a++) {
        for(int c = a + 1; c < n; c++) {
            if (orderScore[c] > orderScore[a]) {
                double tempScore = orderScore[a];
                orderScore[a] = orderScore[c];
                orderScore[c] = tempScore;

                int tempIdx = orderIdx[a];
                orderIdx[a] = orderIdx[c];
                orderIdx[c] = tempIdx;
            }
        }
    }

    fprintf(logFile, "--- B's ATTACK ORDER (E ships within B's attack range, by threat) ---\n");
    for(int r = 0; r < n; r++){

        fprintf(logFile, "  Priority %d: Escort E%d\n", r + 1, escorts[orderIdx[r]].id);
    }
    if (n == 0) {
        fprintf(logFile, "  No escort ships are within B's attack range.\n");
    }
}

//get a random point inside the canvas and move B to it
void moveBattleshipToRandomPoint(Battleship *b, double gridSize){
    double px = (double)(rand() % ((int)gridSize + 1));
    double py = (double)(rand() % ((int)gridSize + 1));

    b->pos.x = px;
    b->pos.y = py;

}

//select the escort (in B's attack range) that poses the greatest current
int selectTargetEscort(Battleship b, EscortShip escorts[], int numEscorts){
    int targetIdx = -1;
    double maxImpact = -1.0;

    for (int i = 0; i < numEscorts; i++) {
        if (escorts[i].destroyed) continue;

        double tof;
        bool bCanHitE = canHitTarget(b.pos, escorts[i].pos, b.vMin, b.vMax,
                                     b.angleMin, b.angleMax, &tof);
        if (!bCanHitE) continue;

        //can the escort currently hit B
        bool eCanHitB = canHitTarget(escorts[i].pos, b.pos, escorts[i].vMin,
                                     escorts[i].vMax, escorts[i].angleMin, escorts[i].angleMax, NULL);

        //current impact of the escort on B
        double currentImpact = getDegradedImpact(escorts[i].impactPower,
                                                 escorts[i].gamma, escorts[i].shotsFired);
        if (eCanHitB) currentImpact += 10.0;

        if (currentImpact > maxImpact){
            maxImpact = currentImpact;
            targetIdx = i;
        }
    }
    return targetIdx;
}

//save the current impact factor of each ship to the log
static void logCurrentImpactFactors(FILE *logFile, Battleship *b, EscortShip escorts[], int numEscorts){
    if (!logFile) return;
    fprintf(logFile, "--- CURRENT IMPACT FACTORS ---\n");
    double bImpact = getDegradedImpact(b->impactPower, b->gamma, b->shotsFired);
    fprintf(logFile, "Battleship (%s): IP=%.4f (base=%.3f, gamma=%.3f, shots=%d)\n",
            b->typeNotation, bImpact, b->impactPower, b->gamma, b->shotsFired);
    for(int i = 0; i < numEscorts; i++) {
        if (escorts[i].destroyed) continue;
        double eImpact = getDegradedImpact(escorts[i].impactPower, escorts[i].gamma, escorts[i].shotsFired);
        fprintf(logFile, "Escort E%d (%s): IP=%.4f (base=%.3f, gamma=%.3f, shots=%d)\n", escorts[i].id, escorts[i].typeNotation, eImpact, escorts[i].impactPower,
                escorts[i].gamma, escorts[i].shotsFired);
    }
}

//combined simulation:continuous fire,  per-type reload times, impact power
void runfullSimulation(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile) {
    double currentTime = 0.0;
    double bNextShotTime = 0.0;
    int escortsRemaining = numEscorts;
    int destroyerId = -1;

    //save initial battlefield conditions to the log file
    if (logFile){
        fprintf(logFile, "--- INITIAL CONDITIONS ---\n");

        fprintf(logFile, "Battleship: %s (%s) at (%.1f, %.1f)\n", b->typeName, b->typeNotation, b->pos.x, b->pos.y);

        fprintf(logFile, "  Vmin=%.1f Vmax=%.1f, angles %.1f-%.1f, impact=%.3f, "
                         "gamma=%.3f, reload=%.1fs\n",
                b->vMin, b->vMax, b->angleMin, b->angleMax, b->impactPower,
                b->gamma, b->reloadTime);

        for (int i = 0; i < numEscorts; i++) {
            if (escorts[i].destroyed) continue;
            fprintf(logFile, "Escort E%d (%s) at (%.1f, %.1f): Vmin=%.1f Vmax=%.1f, "
                             "angles %.1f-%.1f, impact=%.3f, gamma=%.3f, reload=%.1fs\n", escorts[i].id, escorts[i].typeNotation, escorts[i].pos.x, escorts[i].pos.y, escorts[i].vMin, escorts[i].vMax,
                    escorts[i].angleMin, escorts[i].angleMax, escorts[i].impactPower,
                    escorts[i].gamma, escorts[i].reloadTime);
        }

        //attack order of E ships within B's attack range by threat
        logAttackOrder(logFile, *b, escorts, numEscorts);
        fprintf(logFile, "--- SIMULATION START ---\n");
    }

    while(!b->destroyed && escortsRemaining > 0 && currentTime < 1000.0) {
        //battleship firing logic
        if (currentTime >= bNextShotTime) {
            int targetIdx = selectTargetEscort(*b, escorts, numEscorts);
            if (targetIdx != -1) {
                b->shotsFired++;
                double currentBImpact = getDegradedImpact(b->impactPower, b->gamma,
                                                          b->shotsFired);

                //degradation means a ship may need several hits
                escorts[targetIdx].health -= currentBImpact;
                if (escorts[targetIdx].health <= 0.0) {
                    escorts[targetIdx].destroyed = true;
                    escortsRemaining--;
                    if(logFile) {
                        fprintf(logFile, "[t=%.2fs] Battleship fired shot #%d at Escort E%d "
                                         "(Destroyed! Impact=%.3f)\n",
                                currentTime, b->shotsFired, escorts[targetIdx].id,
                                currentBImpact);
                    }
                } else if(logFile) {
                    fprintf(logFile, "[t=%.2fs] Battleship fired shot #%d at Escort E%d "
                                     "(Hit! Damage=%.3f, E%d-health=%.2f%%)\n",
                            currentTime, b->shotsFired, escorts[targetIdx].id,
                            currentBImpact, escorts[targetIdx].id,
                            escorts[targetIdx].health * 100.0);
                }
            } else {
                //no reachable escorts remain; end the battle
                break;
            }
            bNextShotTime = currentTime + b->reloadTime;
        }

        //escort ships firing continouusly
        for (int i = 0; i < numEscorts; i++){
            if (escorts[i].destroyed) continue;

            if (currentTime >= escorts[i].nextFiringTime) {
                double tof;
                if (canHitTarget(escorts[i].pos, b->pos, escorts[i].vMin,
                                 escorts[i].vMax, escorts[i].angleMin,
                                 escorts[i].angleMax, &tof)) {

                    escorts[i].shotsFired++;
                    double currentEImpact = getDegradedImpact(escorts[i].impactPower,
                                                              escorts[i].gamma,
                                                              escorts[i].shotsFired);
                    b->health -= currentEImpact;

                    if (logFile) {
                        fprintf(logFile, "[t=%.2fs] Escort E%d fired shot #%d at Battleship "
                                         "(Hit! Damage=%.2f%%, B-Health=%.2f%%)\n",
                                currentTime, escorts[i].id, escorts[i].shotsFired, currentEImpact * 100.0, b->health * 100.0);
                    }

                    if (b->health <= 0.0) {
                        b->destroyed = true;
                        destroyerId = escorts[i].id;
                        if (logFile){

                            fprintf(logFile, "[t=%.2fs] BATTLESHIP DESTROYED!\n", currentTime);
                        }
                        break;
                    }
                }

                escorts[i].nextFiringTime = currentTime + escorts[i].reloadTime;
            }
        }

        currentTime += 0.5;
    }

    if (logFile){
        fprintf(logFile, "--- SIMULATION END ---\nFinal Battleship Health: %.2f%%\n"
                         "Escorts Destroyed: %d/%d\nDuration: %.2fs\n",
                b->health > 0 ? b->health * 100.0 : 0.0,

                numEscorts - escortsRemaining, numEscorts, currentTime);
        logCurrentImpactFactors(logFile, b, escorts, numEscorts);
    }

    //display the result on the console
    if (b->destroyed){

        printf("Battleship was DESTROYED by Escort E%d after %.2fs.\n",
               destroyerId, currentTime);
    }else{
        printf("Battleship SURVIVED (health %.2f%%). Escorts destroyed: %d/%d over %.2fs.\n", b->health * 100.0, numEscorts - escortsRemaining, numEscorts, currentTime);
    }
}

