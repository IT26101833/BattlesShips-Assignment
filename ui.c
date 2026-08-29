#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "structures.h"

//triggering the simulatoin
void runfullSimulation(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile);

int board_draw_input(void) {
    int grid_value;
    printf("Enter the grid value: ");
    if (scanf("%d", &grid_value) != 1) return 1000;
    return grid_value;
}

//drawing the board
void draw_board(int Grid_Value) {
    for (int i = 0; i < Grid_Value; i++) {
        for (int j = 0; j < Grid_Value; j++) {
            printf("|");
        }
        printf(" ");
    }
    printf("\n");
}

void initial_settingsForBS(Battleship *b) {
    printf("\n--- BATTLESHIP SETUP ---\n");
    printf("Enter Battleship Name: ");
    scanf(" %[^\n]", b->typeName);
    printf("Enter Battleship Notation (e.g., U): ");
    scanf("%4s", b->typeNotation);
    printf("Enter Battleship starting position X: ");
    scanf("%lf", &b->pos.x);
    printf("Enter Battleship starting position Y: ");
    scanf("%lf", &b->pos.y);
    printf("Enter Min Velocity: ");
    scanf("%lf", &b->vMin);
    printf("Enter Max Velocity: ");
    scanf("%lf", &b->vMax);
    printf("Enter Min Angle: ");
    scanf("%lf", &b->angleMin);
    printf("Enter Max Angle: ");
    scanf("%lf", &b->angleMax);
    printf("Enter Base Impact Power (e.g., 0.5): ");
    scanf("%lf", &b->impactPower);
    printf("Enter Gamma (degredation factor): ");
    scanf("%lf", &b->gamma);
    
    b->shotsFired = 0;
    b->health = 1.0;
    b->destroyed = false;
    b->reloadTime = 5.0;
}

void initial_settingsForES(EscortShip escorts[], int *numEscorts, double gridSize) {
    printf("\n--- ESCORT SHIPS SETUP ---\n");
    printf("Enter the number of Escort Ships: ");
    scanf("%d", numEscorts);

    const char *types[] = {"EA", "EB", "EC", "ED", "EE"};
    double baseImpacts[] = {0.08, 0.06, 0.07, 0.05, 0.04};

    for (int i = 0; i < *numEscorts; i++) {
        escorts[i].id = i + 1;
        int typeIdx = rand() % 5;
        strcpy(escorts[i].typeNotation, types[typeIdx]);
        sprintf(escorts[i].typeName, "Escort-%s-%d", types[typeIdx], i + 1);
        
        escorts[i].pos.x = (double)(rand() % (int)gridSize);
        escorts[i].pos.y = (double)(rand() % (int)gridSize);
        escorts[i].vMin = 50.0;
        escorts[i].vMax = 250.0;
        escorts[i].angleMin = 10.0;
        escorts[i].angleMax = 60.0;
        escorts[i].impactPower = baseImpacts[typeIdx];
        escorts[i].gamma = 0.01;
        escorts[i].shotsFired = 0;
        escorts[i].destroyed = false;
        escorts[i].reloadTime = 2.0 + (typeIdx * 0.5);
        escorts[i].nextFiringTime = 0.0;
    }
    printf("Generated %d escort ships successfully.\n", *numEscorts);
}

void view_instructions(void) {
    printf("\n====================================================================================\n");
    printf("-----------------------------------INSTRUCTIONS-------------------------------------\n");
    printf("1. Battleship (B) fires every reload interval at detected escort targets.\n");
    printf("2. Escort ships (E) counter-attack when the battleship enters their range.\n");
    printf("3. Projectile hits use parabolic motion equations.\n");
    printf("4. Shell impact power degrades using IP_n = IP_0 * e^(-gamma * n).\n");
    printf("====================================================================================\n");
}

void view_statistics(void) {
    printf("\n--- SIMULATION STATISTICS ---\n");
    FILE *logFile = fopen("sim_output.txt", "r");
    if (logFile) {
        char ch;
        while ((ch = fgetc(logFile)) != EOF) {
            putchar(ch);
        }
        fclose(logFile);
    } else {
        printf("No past simulation statistics found. Run a simulation first.\n");
    }
}

void setup(EscortShip escorts[], Battleship *b, SimConfig *config) {
    int option = 0;
    do {
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("------------------------------------------------------------------------------------\n");
        printf("====================================================================================\n");
        printf("====================++++++++++++++++SETUP MENU+++++++++++++++=======================\n");
        printf("====================++++++++1.) Battleship Properties++++++++=======================\n");
        printf("====================++++++++2.) Escort ship Properties+++++++=======================\n");
        print("=====================+++++++++++++3.) Seed Value++++++++++++++======================+\n");
        printf("====================++++++++4.) Return to main menu++++++++++=======================\n");
        printf("====================================================================================\n");
        printf("------------------------------------------------------------------------------------\n");
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("Enter option: ");
        if (scanf("%d", &option) != 1) break;

        switch(option) {
            case 1:
                initial_settingsForBS(b);
                break;
            case 2:
                initial_settingsForES(escorts, &config->numEscorts, config->battlefieldSize);
                break;
            case 3:
                printf("Enter Random Seed: ");
                scanf("%u", &config->seed);
                srand(config->seed);
                printf("Enter Grid Size (D): ");
                scanf("%lf", &config->battlefieldSize);
                break;
            case 4:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Option out of range.\n");
        }
    } while(option != 4);
}

void start_simulation_flow(SimConfig *config, Battleship *b) {
    EscortShip *escorts = malloc(sizeof(EscortShip) * config->numEscorts);
    if (!escorts) {
        printf("Memory allocation failed!\n");
        return;
    }

    //initial notation for run
    const char *types[] = {"EA", "EB", "EC", "ED", "EE"};
    double baseImpacts[] = {0.08, 0.06, 0.07, 0.05, 0.04};

    for (int i = 0; i < config->numEscorts; i++) {
        escorts[i].id = i + 1;
        int typeIdx = rand() % 5;
        strcpy(escorts[i].typeNotation, types[typeIdx]);
        sprintf(escorts[i].typeName, "Escort-%s-%d", types[typeIdx], i + 1);
        escorts[i].pos.x = (double)(rand() % (int)config->battlefieldSize);
        escorts[i].pos.y = (double)(rand() % (int)config->battlefieldSize);
        escorts[i].vMin = 50.0;
        escorts[i].vMax = 250.0;
        escorts[i].angleMin = 10.0;
        escorts[i].angleMax = 60.0;
        escorts[i].impactPower = baseImpacts[typeIdx];
        escorts[i].gamma = 0.01;
        escorts[i].shotsFired = 0;
        escorts[i].destroyed = false;
        escorts[i].reloadTime = 2.0 + (typeIdx * 0.5);
        escorts[i].nextFiringTime = 0.0;
    }

    FILE *logFile = fopen("sim_output.txt", "w");
    if (logFile) {
        printf("\nExecuting Simulation... Results outputting to sim_output.txt\n");
        runfullSimulation(b, escorts, config->numEscorts, logFile);
        fclose(logFile);
        printf("Simulation finished successfully.\n");
    } else {
        printf("Failed to open file for logging results.\n");
    }

    free(escorts);
}

void main_menu(SimConfig *config, Battleship *b) {
    int option = 0;
    EscortShip tempEscorts[50];

    do {
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("------------------------------------------------------------------------------------\n");
        printf("------------------------------------------------------------------------------------\n");
        printf("------------------------------------------------------------------------------------\n");
        printf("****************************ADVANCED NAVAL SIMULATOR********************************\n");
        printf("************************************************************************************\n");
        printf("------------------------------------MAIN MENU---------------------------------------\n");
        printf("------------------------------1.) Start Simulation----------------------------------\n");
        printf("-----------------------------2.) View Instructions----------------------------------\n");
        printf("---------------------------3.) Simulation Statistics--------------------------------\n");
        printf("-------------------------------------4.) Exit---------------------------------------\n");
        printf("************************************************************************************\n");
        printf("************************************************************************************\n");
        printf("************************************************************************************\n");
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
        printf("Enter an option: ");
        if (scanf("%d", &option) != 1) break;

        switch(option) {
            case 1: 
                start_simulation_flow(config, b);
                break;
            case 2:
                setup(tempEscorts, b, config);
                break;
            case 3:
                view_instructions();
                break;
            case 4:
                view_statistics();
                break;
            case 5:
                printf("Exiting Program...\n");
                break;
            default:
                printf("Option out of range.\n");
        }
    } while(option != 5);
}