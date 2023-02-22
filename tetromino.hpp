
typedef struct tetromino {
  char type_str[20];
  char piece[4][4];             // 4x4 grid of characters, piece[x][y]
  int upper_left_x;
  int upper_left_y;
  char draw_char;
  char color[3];
} tetromino_t;

extern const tetromino_t tetromino_types[11];

enum {SAFE, COLLIDE}; // Return status for check_collision 
int check_collision (tetromino_t *);

enum {MOVE_OK, MOVE_FAILED}; // Return status for move
int move_tet (tetromino_t *, int, int);
int rotate_cw(tetromino_t *);
int rotate_ccw(tetromino_t *);
tetromino_t *create_tetromino (int, int);
int destroy_tetromino(tetromino_t *);
void print_tetromino(tetromino_t *tet);
void display_tetromino(tetromino_t *tet);
void test_tetromino(void);
void undisplay_tetromino(tetromino_t *tet);
void change_level();

/* tetromino.hpp ends here */
