double degToRad(double deg);
double getDistance(Position p1, Position p2);
double range(double speed, double teta);
bool canHitTarget(Position shooter, Position target, double vMin, double vMax, double aMinDeg, double aMaxDeg, double *timeOfFlight);
double getDegradedImpact(double baseImpact, double gamma, int shotsFired);