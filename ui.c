#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "part1_ui.h"
#include "part2_ui.h"
#include "structures.h"

#define MAX_ESCORTS 50

//randomly generate the escort ship settings
static void generateEscortProperties(EscortShip *e, int typeIdx, double gridSize, double bVMax);

//draw the canvas for game
int board_draw_input(void){
    int grid_value;
    printf("Enter the grid value: ");

    if (scanf("%d", &grid_value) != 1) return 1000;
    return grid_value;
}

//drawing the board
void draw_board(int Grid_Value){
    for(int i = 0; i < Grid_Value; i++) {
        for(int j = 0; j < Grid_Value; j++) {
            printf("|");
        }
        printf(" ");
    }
    printf("\n");
}

//reads a number and keeps asking until it is inside the allowed range
double readInRange(const char *prompt, double min, double max){
    double value;
    int c;

    do{
        printf("%s (%.2f - %.2f): ", prompt, min, max);

        if(scanf("%lf", &value) != 1){
            do{ 
                c = getchar(); 
            } while (c != '\n' && c != EOF); //clear the invalid input
            printf("Invalid input. Please enter a number.\n");


            value = min - 1.0;
        }else if(value < min || value > max) {
            printf("Out of range. Allowed range is %.2f to %.2f.\n", min, max);
        }
     }while(value < min || value > max);

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
        if (b->angleMax <= b->angleMin){

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

void initial_settingsForES(EscortShip escorts[], int *numEscorts, double gridSize, double bVMax){
    printf("\n--- ESCORT SHIPS SETUP ---\n");

    printf("Enter the number of Escort Ships (1-%d): ", MAX_ESCORTS);
    scanf("%d", numEscorts);

    //validate the number of escort ships
    while (*numEscorts < 1 || *numEscorts > MAX_ESCORTS) {
        //print this erro if not in the range
        printf("Invalid number. Please enter a value between 1 and %d: ", MAX_ESCORTS);

        scanf("%d", numEscorts);
    }

    for (int i = 0; i < *numEscorts; i++) {
        escorts[i].id = i + 1;
        int typeIdx = rand() % 5;
        generateEscortProperties(&escorts[i], typeIdx, gridSize, bVMax);
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
    printf("  The simulation can be run as:\n");
    printf("    Part 1-A : instant reload, a single shell destroys any ship\n");
    printf("    Part 1-B : battleship moves along a random k-point path (with\n");
    printf("              optional gun jamming after t iterations, 0<theta<30)\n");
    printf("    Part 1-C : E ships deal partial damage (impact power), B still\n");
    printf("              destroys any E ship with one shell\n");
    printf("    Part 2-A : B reload time (T_B) between firings + a threat-based\n");
    printf("              attack-order strategy; E ships still fire only once\n");
    printf("    Part 2-B : E ships also fire continuously (per-type T_E reload);\n");
    printf("              B keeps its reload time and attack-order strategy\n");
    printf("    Part 2-C : combined - reload times (T_B, T_E), continuous fire,\n");
    printf("              impact-power degradation (IP = IP0*e^-gamma*n)\n");
    printf("  If B sinks, the index of the escort that sank it is shown.\n");
    printf("  If B survives, the number of escorts destroyed and the battle duration\n");
    printf("  are shown. The full log (initial conditions + every shot + B's attack\n");
    printf("  order) is saved to sim_output.txt / sim2A_output.txt / sim2B_output.txt\n");
    printf("  and can be viewed from 'Simulation Statistics'. Part 1-B saves one\n");
    printf("  log file per iteration (sim_path_part1B_iter*.txt / sim_jam* / simC_*\n");
    printf("  sim2A_path_iter*.txt / sim2B_path_iter*.txt).\n");
    printf("  IMPORTANT: velocities are shell muzzle velocities in m/s. Pick a canvas\n");
    printf("  size D and velocities so that the gun ranges (v^2/g) overlap the canvas\n");
    printf("  - otherwise ships cannot reach each other and no battle occurs.\n");
    printf("====================================================================================\n");
}

void view_statistics(void){
    printf("\n==================== SIMULATION STATISTICS ====================\n");
    int total_files_found = 0;

    //read Part 1-B iteration logs (sim_path / sim_jam / simC_path / simC_jam)
    const char *prefixes[] = {"sim_path", "sim_jam", "simC_path", "simC_jam"};
    for(int p = 0; p < 4; p++){
        int iter = 1;
        while(1) {
            char filename[256];
            snprintf(filename, sizeof(filename), "%s_part1B_iter%d.txt", prefixes[p], iter);

            FILE *bFile = fopen(filename, "r");
            if(!bFile) {
                break; //stop checking when no more iteration files exist
            }

            printf("\n--- PART 1-B LOG: %s ---\n", filename);
            char ch;
            while ((ch = fgetc(bFile)) != EOF) {
                putchar(ch);
            }
            fclose(bFile);

            iter++;
            total_files_found++;
        }
    }

    //read Part 2 path iteration logs (sim2A_path / sim2B_path)
    const char *part2Prefixes[] = {"sim2A_path_iter", "sim2B_path_iter"};
    for (int p = 0; p < 2; p++) {
        int iter = 1;
        while (1) {
            char filename[256];
            snprintf(filename, sizeof(filename), "%s%d.txt", part2Prefixes[p], iter);

            FILE *bFile = fopen(filename, "r");
            if (!bFile) {
                break;
            }

            printf("\n--- PART 2 PATH LOG: %s ---\n", filename);
            char ch;
            while ((ch = fgetc(bFile)) != EOF) {
                putchar(ch);
            }
            fclose(bFile);

            iter++;
            total_files_found++;
        }
    }

    //read single-position output files from all parts
    const char *mainFiles[] = {"sim_output.txt", "sim2A_output.txt", "sim2B_output.txt"};
    for (int m = 0; m < 3; m++) {
        FILE *pFile = fopen(mainFiles[m], "r");
        if (pFile) {
            printf("\n--- %s ---\n", mainFiles[m]);
            char ch;
            while ((ch = fgetc(pFile)) != EOF) {
                putchar(ch);
            }
            fclose(pFile);
            total_files_found++;
        }
    }

    if(total_files_found == 0) {
        printf("No past simulation statistics found. Run a simulation first.\n");
    }
    printf("===============================================================\n");
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

                initial_settingsForES(escorts, &config->numEscorts, config->battlefieldSize, b->vMax);
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
    
    if (config->numEscorts < 1){
         config->numEscorts = 1;
    }
    if (config->numEscorts > MAX_ESCORTS) {
        config->numEscorts = MAX_ESCORTS;
    }

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
            escorts[i].id = i + 1;
            int typeIdx = rand() % 5;
            generateEscortProperties(&escorts[i], typeIdx, config->battlefieldSize, b->vMax);
        }
    }

    
    for (int i = 0; i < config->numEscorts; i++) {
        escorts[i].health = 1.0;
        escorts[i].destroyed = false;
        escorts[i].shotsFired = 0;
        
        escorts[i].nextFiringTime = (double)(rand() % 101) / 100.0 * escorts[i].reloadTime;
    }
    printf("\nChoose which part of the simulation to run:\n");

    printf("  1.) Part 1-A  (instant reload, single shot destroys any ship)\n");
    printf("  2.) Part 1-B  (battleship moves along a random k-point path)\n");
    printf("  3.) Part 1-C  (B still one-shots E; E ships only deal partial damage)\n");
    printf("  4.) Part 2-A  (T_B reload + B attack-order strategy; E still fires once)\n");
    printf("  5.) Part 2-B  (T_B and T_E reloads; B and E fire continuously)\n");
    printf("  6.) Part 2-C  (combined: reload times, continuous fire, impact degradation)\n");

printf("Enter option: ");
    int part = 0;
    scanf("%d", &part);

    if(part >= 1 && part <= 3){
        runPart1Simulation(config, b, escorts, part);
    }else if (part >= 4 && part <= 6) {
        runPart2Simulation(config, b, escorts, part);
    }else{
        printf("Invalid part option. Returning to main menu.\n");
    }

    free(escorts);

    
}
//randomly generate the escort ship settings 
static void generateEscortProperties(EscortShip *e, int typeIdx, double gridSize, double bVMax){
    const char *types[] = {"EA", "EB", "EC", "ED", "EE"};
    double baseImpacts[] = {0.08, 0.06, 0.07, 0.05, 0.04};

    double reloads[] = {2.0, 2.5, 2.2, 3.0, 3.5};

    //angle range (thetaH - thetaL) for each escort type, from Table 1
    double angleRanges[] = {20.0, 30.0, 25.0, 50.0, 70.0};

    strcpy(e->typeNotation, types[typeIdx]);

    sprintf(e->typeName, "Escort-%s-%d", types[typeIdx], e->id);
    e->pos.x = (double)(rand() % (int)gridSize);
    e->pos.y = (double)(rand() % (int)gridSize);
    e->impactPower = baseImpacts[typeIdx];
    e->gamma = 0.01 + ((double)(rand() % 90) / 1000.0);

    if (typeIdx == 0) {
        
        e->vMax = 1.2 * bVMax;
        e->vMin = 20.0 + ((double)(rand() % 100));
        if (e->vMin >= e->vMax) e->vMin = e->vMax * 0.5;
    } else {
        
        e->vMin = 20.0 + ((double)(rand() % 100));
        e->vMax = e->vMin + ((double)(rand() % 80));
        if(e->vMax >= bVMax){
            e->vMax = bVMax - 10.0;
        }
        if (e->vMax <= e->vMin){
            e->vMax = e->vMin + 5.0;
        }
    }

    //randomly generate angles within the range
    e->angleMin = 10.0 + (double)(rand() % 10);
    e->angleMax = e->angleMin + angleRanges[typeIdx];

    if(e->angleMax > 90.0){
        e->angleMax = 90.0;
    }
    if (e->angleMax <= e->angleMin){
        e->angleMax = e->angleMin + 5.0;
    }

    e->reloadTime = reloads[typeIdx];
    e->shotsFired = 0;
    e->health = 1.0;
    e->destroyed = false;
    e->nextFiringTime = (double)(rand() % 101) / 100.0 * e->reloadTime;
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
                printf("Are you sure you want to exit? (1=yes, 0=no): ");
                int confirm = 0;
                scanf("%d", &confirm);
                if (confirm == 1) {
                    printf("Exiting Program...\n");
                    option = 4;
                } else {
                    option = 0;
                    printf("Returning to main menu...\n");
                }
                break;
            default:
                printf("Option out of range.\n");
        }
    } while(option != 4);
}