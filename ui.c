#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "structures.h"

#define MAX_ESCORTS 50

//triggering the simulatoin
void runfullSimulation(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile);

//draw the canvas for game
int board_draw_input(void){
    int grid_value;
    printf("Enter the grid value: ");
    if (scanf("%d", &grid_value) != 1) return 1000;
    return grid_value;
}

//drawing the board
void draw_board(int Grid_Value){
    for (int i = 0; i < Grid_Value; i++) {
        for (int j = 0; j < Grid_Value; j++) {
            printf("|");
        }
        printf(" ");
    }
    printf("\n");
}

//reads a number and keeps asking until it is inside the allowed range
static double readInRange(const char *prompt, double min, double max){
    double value;
    int c;

    do {
        printf("%s (%.2f - %.2f): ", prompt, min, max);

        if (scanf("%lf", &value) != 1){
            do { c = getchar(); } while (c != '\n' && c != EOF); //clear the invalid input
            printf("Invalid input. Please enter a number.\n");
            value = min - 1.0;
        } else if (value < min || value > max) {
            printf("Out of range. Allowed range is %.2f to %.2f.\n", min, max);
        }
    } while (value < min || value > max);

    return value;
}

void initial_settingsForBS(Battleship *b, double gridSize){


    printf("\n--- BATTLESHIP SETUP ---\n");
    printf("Available battleship types:\n");
    printf("  Notation | Name                | Gun\n");
    printf("  -------- | ------------------- | ------------------------\n");
    printf("        U  | USS Iowa BB-61      | 50-caliber Mark 7 gun\n");
    printf("        M  | MS King George V    | (356 mm) Mark VII gun\n");
    printf("        R  | Richelieu           | (15 inch) Mle 1935 gun\n");
    printf("        S  | Sovetsky Soyuz-class| (16 inch) B-37 gun\n");
    printf("Enter Battleship Name: ");
    scanf(" %[^\n]", b->typeName);
    printf("Enter Battleship Notation (e.g., U): ");
    scanf("%4s", b->typeNotation);


    //getting the battleship starting position
    b->pos.x = readInRange("Enter Battleship starting position X", 0.0, gridSize);
    b->pos.y = readInRange("Enter Battleship starting position Y", 0.0, gridSize);


    //geting the battleship min velocity
    b->vMin = readInRange("Enter Min Velocity", 0.0, 5000.0);
    do {
        b->vMax = readInRange("Enter Max Velocity", 0.0, 5000.0);
        if (b->vMax <= b->vMin) {
            printf("Out of range. Max Velocity must be greater than Min Velocity (%.2f).\n", b->vMin);
        }
    } while (b->vMax <= b->vMin);

    b->angleMin = readInRange("Enter Min Angle (degrees)", 0.0, 90.0);
    do{
        b->angleMax = readInRange("Enter Max Angle (degrees)", 0.0, 90.0);
        if (b->angleMax <= b->angleMin) {
            printf("Out of range. Max Angle must be greater than Min Angle (%.2f).\n", b->angleMin);
        }
    } while (b->angleMax <= b->angleMin);

    b->impactPower = readInRange("Enter Base Impact Power (e.g., 0.5)", 0.0, 1.0);
    b->gamma = readInRange("Enter Gamma (degradation factor)", 0.0, 1.0);
    b->reloadTime = readInRange("Enter Reload Time (seconds between shots)", 0.1, 60.0);

    b->shotsFired = 0;
    b->health = 1.0;
    b->destroyed = false;
}
//function to initializing escortships
void initial_settingsForES(EscortShip escorts[], int *numEscorts, double gridSize){
    printf("\n--- ESCORT SHIPS SETUP ---\n");
    printf("Enter the number of Escort Ships (1-%d): ", MAX_ESCORTS);
    scanf("%d", numEscorts);

    //validate the number of escort ships
    while (*numEscorts < 1 || *numEscorts > MAX_ESCORTS) {
        //print this erro if not in the range
        printf("Invalid number. Please enter a value between 1 and %d: ", MAX_ESCORTS);

        scanf("%d", numEscorts);
    }

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
        escorts[i].health = 1.0;
        escorts[i].destroyed = false;
        escorts[i].reloadTime = 2.0 + (typeIdx * 0.5);
        escorts[i].nextFiringTime = 0.0;
    }

    printf("Generated %d escort ships successfully.\n", *numEscorts);



}
//function to view instructions
void view_instructions(void){
    printf("-----------------------------------INSTRUCTIONS-------------------------------------\n");
    printf("------------------------------------------------------------------------------------\n");


    printf("------------------------------------------------------------------------------------\n");
    printf("MAIN MENU OPTIONS\n");
    printf("  1. Start Simulation  = open the Setup menu, then run a battle\n");
    printf("  2. View Instructions = show this help screen\n");
    printf("  3. Simulation Stats  = read the last saved results (sim_output.txt)\n");
    printf("  4. Exit              = close the program\n");

    printf("------------------------------------------------------------------------------------\n");
    printf("SETUP MENU OPTIONS\n");
    printf("  1. Battleship Properties  = set the values that control the battleship\n");
    printf("  2. Escort ship Properties = set how many escort ships take part\n");
    printf("  3. Seed value             = set the random generator and canvas size (D)\n");
    printf("  4. Return to main menu    = close this menu; the battle then starts\n");

    printf("------------------------------------------------------------------------------------\n");
    printf("BATTLESHIP SETTINGS AND ALLOWED RANGES\n");
    printf("  Name            : any text (e.g. 'USS Iowa BB-61')\n");
    printf("  Notation        : U, M, R or S\n");
    printf("  Position X, Y   : 0 to D (must be in the given range)\n");
    printf("  Min Velocity    : 0 to 5000.00\n");
    printf("  Max Velocity    : 0 to 5000.00 (must be greater than Min Velocity)\n");
    printf("  Min/Max Angle   : 0 to 90.00 degrees (Max must be greater than Min angle)\n");
    printf("  Impact Power    : 0 to 1.00 (damage dealt by the battleship)\n");
    printf("  Gamma           : 0 to 1.00 (how fast shell power degrades)\n");
    printf("  Reload Time     : 0.10 to 60.00 seconds between shots\n");
    printf("  Entering a value outside a range shows an 'Out of range' message.\n");
    printf("------------------------------------------------------------------------------------\n");
    printf("ESCORT SHIP SETTINGS\n");
    printf("  Number of escorts : 1 to 50. Their type, position, velocity, angles,\n");
    printf("  impact power and reload time are all generated randomly\n");
    printf("  There are 5 types (EA to EE) with different impact powers (0.04 to 0.08).\n");
    printf("------------------------------------------------------------------------------------\n");
    printf("SEED VALUE SETTINGS\n");
    printf("  Seed       : a number (0 - 4294967295) used to generate random values.\n");
    printf("  Grid Size D: 1.00 to 100000.00. The canvas is a square with corners\n");
    printf("               (0, 0) and (D, D)\n");
    printf("------------------------------------------------------------------------------------\n");

    printf("AFTER THE BATTLE\n");
    printf("  If B sinks, the index of the escort that sank it is shown.\n");
    printf("  If B survives, the number of escorts destroyed and the battle duration\n");
    printf("  are shown. The full log (initial conditions + every shot) is saved to\n");
    printf("  sim_output.txt and can be viewed from 'Simulation Statistics'.\n");
    printf("====================================================================================\n");
}

void view_statistics(void){
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

void setup(EscortShip escorts[], Battleship *b, SimConfig *config){
    int option = 0;
    do {
        printf("************************************************************************************\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                   +++++++++++++++SETUP MENU+++++++++++++++                       *\n");
        printf("*                   +++++++1.) Battleship Properties++++++++                       *\n");
        printf("*                   +++++++2.) Escort ship Properties+++++++                       *\n");
        printf("*                   +++++++++++++3.) Seed Value+++++++++++++                       *\n");
        printf("*                   +++++++++4.) Return to main menu++++++++                       *\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("************************************************************************************\n");
        printf("Enter option: ");
        if (scanf("%d", &option) != 1) break;

        switch(option) {
            case 1:
                initial_settingsForBS(b, config->battlefieldSize);
                break;

            case 2:

                initial_settingsForES(escorts, &config->numEscorts, config->battlefieldSize);
                break;

            case 3:
                printf("Enter Random Seed (0 - 4294967295): ");

                scanf("%u", &config->seed);
                srand(config->seed);
                config->battlefieldSize = readInRange("Enter Grid Size (D)", 1.0, 100000.0);
                break;
            case 4:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Option out of range.\n");
        }
    } while(option != 4);
}

//function to start the simulation
void start_simulation_flow(SimConfig *config, Battleship *b, EscortShip customEscorts[]){
    //guard against more escorts than the stored array can hold
    if (config->numEscorts < 1) config->numEscorts = 1;
    if (config->numEscorts > MAX_ESCORTS) config->numEscorts = MAX_ESCORTS;

    EscortShip *escorts = malloc(sizeof(EscortShip) * config->numEscorts);
    if (!escorts) {
        printf("Memory allocation failed!\n");
        return;
    }

    //reset battleship state so it starts each simulation fresh
    b->health = 1.0;
    b->destroyed = false;
    b->shotsFired = 0;

    //copy user desired configurations or genreate randomely
    for (int i = 0; i < config->numEscorts; i++) {
        if (customEscorts[i].id != 0) {
            escorts[i] = customEscorts[i];
        } else {
            //random generation logic
            const char *types[] = {"EA", "EB", "EC", "ED", "EE"};
            double baseImpacts[] = {0.08, 0.06, 0.07, 0.05, 0.04};
            
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
            escorts[i].nextFiringTime = (double)(rand() % 101) / 100.0 * escorts[i].reloadTime;
        }
    }

    //reset per each run
    for (int i = 0; i < config->numEscorts; i++) {
        escorts[i].health = 1.0;
        escorts[i].destroyed = false;
        escorts[i].shotsFired = 0;
        //random aim delay before the first shot
        escorts[i].nextFiringTime = (double)(rand() % 101) / 100.0 * escorts[i].reloadTime;
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

void main_menu(SimConfig *config, Battleship *b){
    int option = 0;
    EscortShip tempEscorts[MAX_ESCORTS] = {0};

    do {
        printf("************************************************************************************\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                           ADVANCED NAVAL SIMULATOR                               *\n");
        printf("*                                                                                  *\n");
        printf("*-----------------------------------MAIN MENU--------------------------------------*\n");
        printf("*-----------------------------1.) Start Simulation---------------------------------*\n");
        printf("*----------------------------2.) View Instructions---------------------------------*\n");
        printf("*--------------------------3.) Simulation Statistics-------------------------------*\n");
        printf("*------------------------------------4.) Exit--------------------------------------*\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("*                                                                                  *\n");
        printf("************************************************************************************\n");
        printf("Enter an option: ");
        if (scanf("%d", &option) != 1) break;

        switch(option) {
            case 1: 
                setup(tempEscorts, b, config);
                start_simulation_flow(config, b, tempEscorts);
                break;
            case 2:
                view_instructions();
                break;
            case 3:
                view_statistics();
                break;
            case 4:
                printf("Exiting Program...\n");
                break;
            default:
                printf("Option out of range.\n");
        }
    } while(option != 4);
}