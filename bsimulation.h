int runPart1A(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile, bool escortsDealPartialDamage, int *destroyerOut);
void runPart1B(Battleship *b, EscortShip escorts[], int numEscorts, int k, double jamAfterIterations, double jamAngleDeg, const char *baseFile, double gridSize, bool escortsDealPartialDamage);
int selectTargetEscort(Battleship b, EscortShip escorts[], int numEscorts);
void runfullSimulation(Battleship *b, EscortShip escorts[], int numEscorts, FILE *logFile);