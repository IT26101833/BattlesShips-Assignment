#include <stdio.h>
#include "part2.h"
#include "bsimulation.h"
#include "part2_ui.h"
#include "structures.h"

//Part 2-A and Part 2-B re-simulate the Part 1 scenarios with reload times.
static void runPart2Scenario(int part, Battleship *b, EscortShip escorts[], int numEscorts, double gridSize){
    int scenario = 0;
    printf("\nChoose which Part 1 scenario to re-simulate with Part %d-%c features:\n",
           part, part == 2 ? 'A' : 'B');
    printf("  1.) Part 1-A scenario  (single position; one E hit destroys B)\n");

    printf("  2.) Part 1-B scenario  (random k-point path and one E hit destroys B)\n");
    printf("  3.) Part 1-C scenario  (single position and E ships deals partial damage)\n");
    printf("  4.) Part 1-C path      (random k-point path with partial damage)\n");
    printf("Enter option: ");
    
    if (scanf("%d", &scenario) != 1) return;

    if (scenario < 1 || scenario > 4) {
        scenario = 1;
    }

    int k = 1;
    if (scenario == 2 || scenario == 4) {
        printf("Enter the number of path points (k): ");
        scanf("%d", &k);
        if (k < 1) k = 1;
    }

    bool escortsDealPartialDamage = (scenario == 3 || scenario == 4);

    if(part == 2) {

        runPart2A(b, escorts, numEscorts, k, gridSize, escortsDealPartialDamage);
    } else {
        runPart2B(b, escorts, numEscorts, k, gridSize, escortsDealPartialDamage);
    }
}

//runs part 2-A, 2-B or 2-C based on the option picked by the part menu
void runPart2Simulation(SimConfig *config, Battleship *b, EscortShip escorts[], int part){
    if (part == 4) {
        runPart2Scenario(2, b, escorts, config->numEscorts, config->battlefieldSize);
    }else if (part == 5){

        runPart2Scenario(3, b, escorts, config->numEscorts, config->battlefieldSize);
    } else{

        FILE *logFile = fopen("sim_output.txt", "w");
        if (logFile){
            printf("\nExecuting Simulation... Results outputting to sim_output.txt\n");

            runfullSimulation(b, escorts, config->numEscorts, logFile);
            fclose(logFile);
            printf("Simulation finished successfully.\n");

        }else {

            printf("Failed to open file for logging results.\n");
        }
    }
}