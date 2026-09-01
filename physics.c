#include <math.h>
#include <stdio.h>
#include "structures.h"

#define GRAVITY 10
#define PI 3.14159


//converting degrees to pi
double degToRad(double deg){
    return deg * (PI / 180.0);
}


double getDistance(Position p1, Position p2){
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    return sqrt(pow(dx,2) + pow(dy,2));
}

//determinig if the target can be reached
bool canHitTarget(Position shooter, Position target, double vMin, double vMax, double aMinDeg, double aMaxDeg, double *timeOfFlight) {
    double R = getDistance(shooter, target);
    double aMin = degToRad(aMinDeg);
    double aMax = degToRad(aMaxDeg);

    
    for (double v = vMin; v <= vMax; v += 1.0) {
        double val = (R * GRAVITY) / (v * v);
        if (val <= 1.0) {
            double theta = 0.5 * asin(val);
            if (theta >= aMin && theta <= aMax) {
                if (timeOfFlight != NULL) {
                    *timeOfFlight = (2.0 * v * sin(theta)) / GRAVITY;
                }
                return true;
            }
        }
    }
    return false;
}
double range(double speed, double teta){
	return (pow(speed, 2)* sin(2 * teta))/GRAVITY;
}

//calculating impact
double getDegradedImpact(double baseImpact, double gamma, int shotsFired) {
    return baseImpact * exp(-gamma * (double)shotsFired);
}