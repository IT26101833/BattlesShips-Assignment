#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <BattleShipFuncDecl.h>
#include <BattleShipFunc.c>

typedef struct {

	char typeNotation[3];
	char typeName[30];
	char gunName[30];
	float impactPower;
	int angleRange;
	int minimumAngles;
	int minimumVelocity;
	int maximumVelocity;

} ES;

typedef struct {

	char bshipName[20];
	char notation;
	char gunName[25];
} BS;

int main(){

	srand((unsigned int)time(NULL));

	ES Eships[5];
	main_menu(Eships);

	return 0;
}
