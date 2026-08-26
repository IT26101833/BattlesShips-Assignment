#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <BattleShipFuncDecl.h>
#include <physics.h>

#define GRAVITY 10
#define PI 3.14159


int main(){

	srand((unsigned int)time(NULL));

	ES Eships[5];
	main_menu(Eships);

	return 0;
}
