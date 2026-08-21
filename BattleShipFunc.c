#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initializingBS(BS BShips[4]){
	const char *BattleshipName[4] = {"USS Iowa BB-61", "MS King George V", "Richelieu", "Sovetsky Soyuz-class"};
	const char *BattleshipNotat[4] = {"U","M","R","S"};
	const char *GunName[4] = {"50-caliber Mark 7 gun","(356 mm) Mark VII gun","(15 inch) Mle 1935 gun", "(16 inch) B-37 gun"};
	
	printf("--------------------------------BattleShips Details-----------------------------------\n");
	printf("--------------------------------------------------------------------------------------\n");

	for(int i = 0; i < 4; i++){
		printf("Name: %s | Notation : %s | Gun Name: %s ", BattleshipName[i], BattleshipNotat[i], GunName[i]);
	}

}

void initializingES(ES EShips[5]){
	const char *notations[5] = {"EA", "EB", "EC", "ED", "EE"};
	const char *names[5] = {"1936A-class-Destroyer", "Gabbiano-class-Corvette", "Matsu-class-Destroyer", "F-class-Escort-Ships", "Japanese-Kaibōkan"};
	const char *gun[5] = {"SK C/34naval gun", "L/47 dualpurpose gun", "Type 89 dualpurpose gun", "SK C/32 naval gun", "(4.7 inch) naval guns"};
	float impact[5] = {0.08, 0.06, 0.07, 0.05, 0.04};
	int angle_Range[5] = {20,30,25,50,70};
	

	int minVAL, maxVAL;

	printf("----------------Setup Minimum Angles-----------------\n");
	getMinMAx(&minVAL, &maxVAL);
	for(int i = 0; i < 5; i++){
		EShips[i].minimumAngles = getRandom(minVAL, maxVAL);

	}

	printf("---------------Setuo Minimum Velocities---------------\n");
	getMinMAx(&minVAL, &maxVAL);
	for(int i = 0; i < 5; i++){
		EShips[i].minimumVelocity = getRandom(minVAL, maxVAL);
	}
	
	printf("---------------Setup Maximum Velocities----------------\n");
	getMinMAx(&minVAL, &maxVAL);
	for(int i = 0; i < 5; i++){
		EShips[i].maximumVelocity = getRandom(minVAL, maxVAL);
	}

	printf("=========================================================\n");
	printf("===================GENERATED SHIP DATA===================\n");
	for(int i = 0; i < 5; i++){
		strcpy(EShips[i].typeNotation, notations[i]);
		strcpy(EShips[i].typeName, names[i]);
		strcpy(EShips[i].gunName, gun[i]);
		EShips[i].impactPower = impact[i];
		EShips[i].angleRange = angle_Range[i];
	}

	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("***********************************************************\n");
	printf("--------------------Displaying Ship Data-------------------\n");

	for(int i = 0; i < 5; i++){
		printf("Escort Ships Notation: [%c] , Ship Name: %s , Ship Weapon: %s", EShips[i].typeNotation, EShips[i].typeName, EShips[i].gunName);
		printf("Min Angle: %d , Min Velocity: %d , Max Velocity: %d", EShips[i].minimumAngles, EShips[i].minimumVelocity, EShips[i].maximumVelocity);
	}
	printf("++++++++++=======================================+++++++++++\n");
}

void getMinMAx(int *min, int *max){
	printf("Enter the min value: ");
	scanf("%d", min);
	printf("Enter the max value: ");
	scanf("%d", max);
}

int getRandom(int min, int max){
		if(min >= max) return min;
		return (rand() % (max - min + 1)) + min; 
}

void Canvas(int dim_x, int dim_y){

	printf("------------------------------------------------\n");
	printf("---------------Drawing BattleField--------------\n");
	printf("---------------*******************--------------\n");
	int EscortShips;
	printf("Enter the total number of Escort ships: ");
	scanf("%d", &EscortShips);

	//setting the size of the canvas
	
	printf("-------Enter the BattleFiled Dimmensions-------\n");
	printf("-------*********************************-------\n");

	printf("Enter the X dimension value: ");
	scanf("%d", &dim_x);
	printf("Enter the Y dimension value: ");
	scanf("%d", &dim_y);
}

void main_menu(ES *EShips[5]){

	int option = 0;

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
		printf("-----------------------------------4.) Exit-----------------------------------------\n");
		printf("************************************************************************************\n");
		printf("************************************************************************************\n");
		printf("************************************************************************************\n");
		printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("Enter a option you choose\n");
		scanf("%d", &option);

		switch(option){
			case 1: 

					initializingES(EShips);
					break;
			case 2:
					printf("Instructions Setup\n");
					break;
			case 3:
					printf("No past Statistics available yet.\n");
					break;
			case 4:
					printf("Exiting Program\n");
					break;
			default:
					printf("Option put of range\n");
		}
	} while(option != 4);

	

}

void setup(ES *EShips){
	 int option;
	do{
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

		switch(option){
			case 1:
				break;
			case 2:
				initializingES(ES EShips[5]);
				break;
		}

	}while(option != 4);

}
