int board_draw_input(void);
void draw_board(int Grid_Value);
void initial_settingsForBS(Battleship *b);
void initial_settingsForES(EscortShip escorts[], int *numEscorts, double gridSize);
void view_instructions(void);
void view_statistics(void);
void setup(EscortShip escorts[], Battleship *b, SimConfig *config);
void start_simulation_flow(SimConfig *config, Battleship *b);
void main_menu(SimConfig *config, Battleship *b);
