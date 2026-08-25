#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <BattleShipFuncDecl.h>
#include <physics.h>

typedef struct {
	const char *type;
	int positionX;
	int positionY;
	float angle;
	double vMin;
	double vMax;
	double gamma;
	int shotsFired;
	int destroyed;

} BS;

typedef struct {

	char *ID;
	char type;
	int positionX;
	int positionY;
	double vMIn;
	double vMax;
} ES;

int main(){

	srand((unsigned int)time(NULL));

	ES Eships[5];
	main_menu(Eships);

	return 0;
}
