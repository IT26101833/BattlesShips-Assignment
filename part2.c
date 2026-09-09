#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "physics.h"
#include "part2.h"
#include "bsimulation.h"
#include "structures.h"

//Part 2-A: B fires with a reload timeand  attack

static void runPart2ASingle(Battleship *b, EscortShip escorts[], int numEscorts,
                            bool escortsDealPartialDamage, FILE *logFile){
    double currentTime = 0.0;
    int escortsRemaining = numEscorts;
    int destroyerId = -1;

    //save the strategy selected attack order before firing begins
    logAttackOrder(logFile, *b, escorts, numEscorts);

    //engage the highest-priority reachable escort
    while (escortsRemaining > 0) {
        int targetIdx = selectTargetEscort(*b, escorts, numEscorts);
        if (targetIdx == -1) break;   //nothing left in B's attack range

        double tof;
        canHitTarget(b->pos, escorts[targetIdx].pos, b->vMin, b->vMax, b->angleMin, b->angleMax, &tof);

        currentTime += tof;
        b->shotsFired++;
        escorts[targetIdx].destroyed = true;
        escortsRemaining--;

        if (logFile) {
            fprintf(logFile, "[t=%.2fs] Battleship fired shot #%d at Escort E%d "
                             "(Index=%d, Time-to-hit=%.2fs). E destroyed.\n",
                    currentTime, b->shotsFired, escorts[targetIdx].id,
                    escorts[targetIdx].id, tof);
        }

        //wait the reload time before the next gun firing
        currentTime += b->reloadTime;
    }

    //each surviving E ship uses its single shot against B
    if (escortsRemaining > 0 && !b->destroyed) {
        for (int i = 0; i < numEscorts; i++) {
            if (escorts[i].destroyed) continue;

            double tof;
            bool eCanHitB = canHitTarget(escorts[i].pos, b->pos, escorts[i].vMin, escorts[i].vMax, escorts[i].angleMin, escorts[i].angleMax, &tof);
            if (!eCanHitB) continue;

            currentTime += tof;
            escorts[i].shotsFired++;

            if (escortsDealPartialDamage) {
                b->health -= escorts[i].impactPower;
                if (logFile) {
                    fprintf(logFile, "[t=%.2fs] Escort E%d fired its single shot at "
                                     "Battleship (Impact=%.3f, B-Health=%.2f%%).\n",
                            currentTime, escorts[i].id, escorts[i].impactPower,
                            b->health > 0 ? b->health * 100.0 : 0.0);
                }
                if (b->health <= 0.0) {
                    b->health = 0.0;
                    b->destroyed = true;
                    destroyerId = escorts[i].id;
                    if (logFile) {
                        fprintf(logFile, "[t=%.2fs] BATTLESHIP DESTROYED by cumulative impact.\n", currentTime);
                    }
                    break;
                }
            } else {
                b->health = 0.0;
                b->destroyed = true;
                destroyerId = escorts[i].id;
                if (logFile) {
                    fprintf(logFile, "[t=%.2fs] Escort E%d hit Battleship (single shot). "
                                     "B is DESTROYED.\n", currentTime, escorts[i].id);
                }
                break;
            }
        }
    }

    if (logFile) {
        fprintf(logFile, "--- PART 2-A RESULT ---\n");
        if (b->destroyed) {
            fprintf(logFile, "Battleship sank. Sunk by Escort E%d. Duration=%.2fs.\n", destroyerId, currentTime);
        } else {
            fprintf(logFile, "Battleship survived. Escorts destroyed: %d/%d. Duration=%.2fs.\n", numEscorts - escortsRemaining, numEscorts, currentTime);
            if(escortsDealPartialDamage) {

                fprintf(logFile, "Cumulative impact on B: %.2f%%.\n", b->health * 100.0);
            }
        }
    }

    if(b->destroyed) {
        printf("Part 2-A: Battleship DESTROYED by Escort E%d after %.2fs.\n",
               destroyerId, currentTime);
    } else{

        printf("Part 2-A: Battleship SURVIVED (health %.2f%%). Escorts destroyed: %d/%d over %.2fs.\n", b->health > 0 ? b->health * 100.0 : 0.0,
               numEscorts - escortsRemaining, numEscorts, currentTime);
    }
}

// battlesship goes to k = number of path points.
void runPart2A(Battleship *b, EscortShip escorts[], int numEscorts, int k, double gridSize, bool escortsDealPartialDamage){
    if (k <= 1) {
        FILE *logFile = fopen("sim2A_output.txt", "w");
        if (!logFile) {

            printf("Failed to open sim2A_output.txt for logging.\n");
            return;
        }
        fprintf(logFile, "--- PART 2-A (single position): INITIAL CONDITIONS ---\n");
        writeInitialConditions(logFile, b, escorts, numEscorts);

        runPart2ASingle(b, escorts, numEscorts, escortsDealPartialDamage, logFile);
        fclose(logFile);

        printf("Part 2-A finished. Results saved to sim2A_output.txt\n");
        return;
    }

    //repeat the battle along k random path points
    for (int iter = 0; iter < k; iter++){
        moveBattleshipToRandomPoint(b, gridSize);

        char fname[256];
        snprintf(fname, sizeof(fname), "sim2A_path_iter%d.txt", iter + 1);
        FILE *logFile = fopen(fname, "w");
        if (!logFile) {
            printf("Failed to open %s for logging.\n", fname);
            return;
        }

        fprintf(logFile, "--- PART 2-A PATH (iteration %d of %d) ---\n", iter + 1, k);
        fprintf(logFile, "Battleship at (%.1f, %.1f)\n", b->pos.x, b->pos.y);
        writeInitialConditions(logFile, b, escorts, numEscorts);
        runPart2ASingle(b, escorts, numEscorts, escortsDealPartialDamage, logFile);
        fclose(logFile);

        if (b->destroyed) {

            printf("Part 2-A path: Battleship sank at iteration %d. Simulation stopped.\n", iter + 1);
            return;
        }
    }
    printf("Part 2-A path finished. Results saved as sim2A_path_iter*.txt\n");
}

//Part 2-B: battlehsip fires with reload time T_B and E ships fire continuously.
static void runPart2BSingle(Battleship *b, EscortShip escorts[], int numEscorts, bool escortsDealPartialDamage, FILE *logFile){
    double currentTime = 0.0;
    double bNextShotTime = 0.0;
    int escortsRemaining = numEscorts;
    int destroyerId = -1;

    //save the strategy-selected attack order before firing begins
    logAttackOrder(logFile, *b, escorts, numEscorts);

    while (!b->destroyed && escortsRemaining > 0 && currentTime < 1000.0) {
        //battleship firing logic with attack-order strategy
        if (currentTime >= bNextShotTime) {
            int targetIdx = selectTargetEscort(*b, escorts, numEscorts);
            if (targetIdx != -1){

                b->shotsFired++;
                escorts[targetIdx].destroyed = true;
                escortsRemaining--;

                if (logFile) {
                    fprintf(logFile, "[t=%.2fs] Battleship fired shot #%d at Escort E%d "
                                     "(Destroyed! Impact=1.000)\n",
                            currentTime, b->shotsFired, escorts[targetIdx].id);
                }
            }
            bNextShotTime = currentTime + b->reloadTime;
        }

        //every surviving E ship fires continuously with its own reload time T_E
        for (int i = 0; i < numEscorts; i++) {
            if (escorts[i].destroyed) continue;

            if (currentTime >= escorts[i].nextFiringTime) {
                double tof;
                if (canHitTarget(escorts[i].pos, b->pos, escorts[i].vMin,
                                 escorts[i].vMax, escorts[i].angleMin,
                                 escorts[i].angleMax, &tof)) {
                    escorts[i].shotsFired++;

                    if (escortsDealPartialDamage) {
                        b->health -= escorts[i].impactPower;
                        if (logFile) {
                            fprintf(logFile, "[t=%.2fs] Escort E%d fired shot #%d at "
                                             "Battleship (Impact=%.3f, B-Health=%.2f%%).\n",
                                    currentTime, escorts[i].id, escorts[i].shotsFired,
                                    escorts[i].impactPower,
                                    b->health > 0 ? b->health * 100.0 : 0.0);
                        }
                        if (b->health <= 0.0) {
                            b->health = 0.0;
                            b->destroyed = true;
                            destroyerId = escorts[i].id;
                            if (logFile) {
                                fprintf(logFile, "[t=%.2fs] BATTLESHIP DESTROYED by cumulative impact.\n", currentTime);
                            }
                            break;
                        }
                    } else {
                        b->health = 0.0;
                        b->destroyed = true;
                        destroyerId = escorts[i].id;
                        if (logFile) {
                            fprintf(logFile, "[t=%.2fs] Escort E%d hit Battleship "
                                             "(single shot). B is DESTROYED.\n",
                                    currentTime, escorts[i].id);
                        }
                        break;
                    }
                }

                escorts[i].nextFiringTime = currentTime + escorts[i].reloadTime;
            }
        }

        currentTime += 0.5;
    }

    if (logFile) {
        fprintf(logFile, "--- PART 2-B RESULT ---\n");
        if (b->destroyed) {
            fprintf(logFile, "Battleship sank. Sunk by Escort E%d. Duration=%.2fs.\n",
                    destroyerId, currentTime);
        } else {
            fprintf(logFile, "Battleship survived. Escorts destroyed: %d/%d. Duration=%.2fs.\n",
                    numEscorts - escortsRemaining, numEscorts, currentTime);
            if (escortsDealPartialDamage) {
                fprintf(logFile, "Cumulative impact on B: %.2f%%.\n", b->health * 100.0);
            }
        }
    }

    if (b->destroyed) {
        printf("Part 2-B: Battleship DESTROYED by Escort E%d after %.2fs.\n",
               destroyerId, currentTime);
    } else {
        printf("Part 2-B: Battleship SURVIVED (health %.2f%%). Escorts destroyed: %d/%d over %.2fs.\n",
               b->health > 0 ? b->health * 100.0 : 0.0,
               numEscorts - escortsRemaining, numEscorts, currentTime);
    }
}

//battleship moves with k point iterations
void runPart2B(Battleship *b, EscortShip escorts[], int numEscorts, int k,
               double gridSize, bool escortsDealPartialDamage){
    if (k <= 1) {
        FILE *logFile = fopen("sim2B_output.txt", "w");
        if (!logFile) {
            printf("Failed to open sim2B_output.txt for logging.\n");
            return;
        }
        fprintf(logFile, "--- PART 2-B (single position): INITIAL CONDITIONS ---\n");

        writeInitialConditions(logFile, b, escorts, numEscorts);
        runPart2BSingle(b, escorts, numEscorts, escortsDealPartialDamage, logFile);
        fclose(logFile);
        printf("Part 2-B finished. Results saved to sim2B_output.txt\n");
        return;
    }

    //repeat the same battle along  with k random path points
    for (int iter = 0; iter < k; iter++) {
        moveBattleshipToRandomPoint(b, gridSize);

        char fname[256];
        snprintf(fname, sizeof(fname), "sim2B_path_iter%d.txt", iter + 1);
        FILE *logFile = fopen(fname, "w");
        if (!logFile) {
            printf("Failed to open %s for logging.\n", fname);
            return;
        }

        fprintf(logFile, "--- PART 2-B PATH (iteration %d of %d) ---\n", iter + 1, k);
        fprintf(logFile, "Battleship at (%.1f, %.1f)\n", b->pos.x, b->pos.y);
        writeInitialConditions(logFile, b, escorts, numEscorts);
        runPart2BSingle(b, escorts, numEscorts, escortsDealPartialDamage, logFile);
        fclose(logFile);

        if (b->destroyed) {
            
            printf("Part 2-B path: Battleship sank at iteration %d. Simulation stopped.\n", iter + 1);
            return;
        }
    }
    printf("Part 2-B path finished. Results saved as sim2B_path_iter*.txt\n");
}