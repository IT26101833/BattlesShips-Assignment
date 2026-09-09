#include <stdio.h>
#include "ui.h"
#include "part1.h"
#include "part1_ui.h"
#include "structures.h"

//runs part 1-A, 1-B or 1-C based on the option picked in the part menu
void runPart1Simulation(SimConfig *config, Battleship *b, EscortShip escorts[], int part){
    if(part == 1){
        FILE *logFile = fopen("sim_output.txt", "w");
        if (logFile){

            fprintf(logFile, "--- PART 1-A: INITIAL CONDITIONS ---\n");
            fprintf(logFile, "Battleship: %s (%s) at (%.1f, %.1f) Vmax=%.1f Vmin=%.1f "
                             "angles %.1f-%.1f\n",
                    b->typeName, b->typeNotation, b->pos.x, b->pos.y, b->vMax, b->vMin,
                    b->angleMin, b->angleMax);
            for(int i = 0; i < config->numEscorts; i++){


                fprintf(logFile, "Escort E%d (%s) at (%.1f, %.1f): Vmin=%.1f Vmax=%.1f "
                                 "angles %.1f-%.1f type=%s\n", escorts[i].id, escorts[i].typeNotation, escorts[i].pos.x, escorts[i].pos.y, escorts[i].vMin, escorts[i].vMax,
                        escorts[i].angleMin, escorts[i].angleMax, escorts[i].typeName);
            }
            printf("\nExecuting Part 1-A simulation...\n");
            int sinker = -1;

            runPart1A(b, escorts, config->numEscorts, logFile, false, &sinker);
            fclose(logFile);

            printf("Part 1-A finished. Results saved to sim_output.txt\n");
        } else{

            printf("Failed to open file for logging results.\n");
        }
    } else if (part == 2) {
        int k = 0;

        printf("Enter the number of path points (k): ");
        scanf("%d", &k);

        if (k < 1) k = 1;

        printf("Run with gun jamming (Simulation 2 - 0<theta<30)? (1=yes, 0=no): ");
        int jam = 0;
        scanf("%d", &jam);

        double jamAngle = 0.0;
        int jamAfter = 1;
        if (jam == 1) {
            jamAngle = readInRange("Enter jam angle theta_jam (degrees)", 1.0, 29.0);
            printf("Enter the iteration t after which the gun jams (1-%d): ", k - 1);

            scanf("%d", &jamAfter);
            if (jamAfter < 1) jamAfter = 1;
            if (jamAfter >= k) jamAfter = k - 1;
        }

        runPart1B(b, escorts, config->numEscorts, k, (double)jamAfter, jamAngle, jam == 1 ? "sim_jam" : "sim_path", config->battlefieldSize, false);
        printf("\nPart 1-B finished. Iteration files saved as sim_path_part1B_iter*.txt "
               "/ sim_jam_part1B_iter*.txt\n");
    }else if (part == 3) {

        printf("Do you want to run Part 1-C over a single position (1) or a "
               "random k-point path (2)? ");
        int mode = 0;
        scanf("%d", &mode);

        if (mode == 2) {
            int k = 0;
            printf("Enter the number of path points (k): ");
            scanf("%d", &k);

            if (k < 1) k = 1;
            printf("Run with gun jamming (0<theta<30)? (1=yes, 0=no): ");
            int jam = 0;
            scanf("%d", &jam);

            double jamAngle = 0.0;
            int jamAfter = 1;
            if (jam == 1){

                jamAngle = readInRange("Enter jam angle theta_jam (degrees)", 1.0, 29.0);

                printf("Enter the iteration t after which the gun jams (1-%d): ", k - 1);
                scanf("%d", &jamAfter);

                if (jamAfter < 1) jamAfter = 1;
                if (jamAfter >= k) jamAfter = k - 1;
            }
            runPart1B(b, escorts, config->numEscorts, k, (double)jamAfter, jamAngle,
                      jam == 1 ? "simC_jam" : "simC_path",
                      config->battlefieldSize, true);

            printf("\nPart 1-C path finished. Iteration files saved to text files.\n");

        } else {
            FILE *logFile = fopen("sim_output.txt", "w");
            if (logFile) {
                fprintf(logFile, "--- PART 1-C: INITIAL CONDITIONS ---\n");
                fprintf(logFile, "Battleship: %s (%s) at (%.1f, %.1f) Vmax=%.1f Vmin=%.1f "
                                 "angles %.1f-%.1f impact=%.3f\n",
                        b->typeName, b->typeNotation, b->pos.x, b->pos.y, b->vMax,
                        b->vMin, b->angleMin, b->angleMax, b->impactPower);
                for (int i = 0; i < config->numEscorts; i++){
                    fprintf(logFile, "Escort E%d (%s) at (%.1f, %.1f): Vmin=%.1f Vmax=%.1f "
                                     "angles %.1f-%.1f impact=%.3f type=%s\n", escorts[i].id, escorts[i].typeNotation, escorts[i].pos.x, escorts[i].pos.y, escorts[i].vMin, escorts[i].vMax,
                            escorts[i].angleMin, escorts[i].angleMax,
                            escorts[i].impactPower, escorts[i].typeName);
                }
                printf("\nExecuting Part 1-C simulation...\n");
                int sinker = -1;

                runPart1A(b, escorts, config->numEscorts, logFile, true, &sinker);
                fclose(logFile);

                printf("Part 1-C finished. Results saved to sim_output.txt\n");
            }else {

                printf("Failed to open file for logging results.\n");
            }
        }
    }
}