typedef struct terminal_dimensions {
  int maxx;
  int maxy;
} terminal_dimensions_t;

// Delay timers for the main game loop.
#ifndef DELAY_US
#define DELAY_US 100000
#endif

// Game States
enum {INIT,NAME, ADD_PIECE, MOVE_PIECE, LIFE_LOST, ADJUST_WELL, PAUSE,  GAME_OVER, EXIT};

void init_game(void);

int game(void);


/* game.hpp ends here */
