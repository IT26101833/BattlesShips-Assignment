#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdbool.h>


//structure fro battleship possition
typedef struct {
    double x;
    double y;
} Position;

//structure for batleship
typedef struct {
    char typeName[30];
    char typeNotation[5];
    Position pos;
    double vMin;
    double vMax;
    double angleMin;
    double angleMax;
    double impactPower;
    double gamma;
    int shotsFired;
    double health;
    bool destroyed;
    double reloadTime; 
} Battleship;


//structure for escortship
typedef struct {
    int id;
    char typeNotation[5]; 
    char typeName[30];
    Position pos;
    double vMin;
    double vMax;
    double angleMin;
    double angleMax;
    double impactPower;
    double gamma;
    int shotsFired;
    bool destroyed;
    double reloadTime; 
    double nextFiringTime;
} EscortShip;


//sttructure for input values
typedef struct {
    double battlefieldSize;
    unsigned int seed;
    int numEscorts;
} SimConfig;

#endif
