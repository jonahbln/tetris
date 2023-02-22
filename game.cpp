/* game.cpp --- 
 * 
 * Filename: game.cpp
 * Description: 
 * Author: Bryce
 * Maintainer: Adeel Bhutta
 * Created: Tue Sep  6 11:08:59 2016
 * Last-Updated: 10-18-2022
 *           By: Jonah Nidorf and Adam Coussin
 *     Update #: 21
 * Keywords: 
 * Compatibility: Linux
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 *  up arrow functionality
 * down arrow functionality
 * left and right functionality
 * made x and q quit the game
 * implemented fall rate
 * implemented spacebar drop
 * implemented fast drop rate
 * pause game functionality
 * created 4 new tetrominos
 * randomized drop rates for tetrominos
 * game ovr conditions + screen
 * take in a name and display it during game
 * dislay lives, score 
 * 
 */

/* Copyright (c) 2016 IUB
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

// PARTNERS: jonah nidorf and adam coussin


/* Code: */
#include <unistd.h> 
#include <ncurses.h>
#include <ctime>
#include <cstdlib>
#include "game.hpp"
#include "well.hpp"
#include "tetris.hpp"
#include "tetromino.hpp"
#include "key.hpp"
#include "score.hpp"
#include <stdio.h>
#include <string>
using namespace std;

void init_game(void) {
  int x,y;
}

int game(void) {
 static int state = INIT;
  tetromino_t *next = NULL;
  tetromino_t *current = NULL;
  well_t *w;
  int x,y;
  int c;
  int arrow;
  struct timespec tim = {0,1000000};  // Each execution of while(1) is approximately 1mS
  struct timespec tim_ret;
  int move_counter = 0;
  int move_timeout = BASE_FALL_RATE;
  char* name = new char[20];
  int i = 0;
  int row_to_clear = 1; //signals the row to clear, starting from the bottom row (1)
  int score = 0;
  int lives = 3;
  int timer_m = 0;
  int timer_s = 0;
  int level = 1;
  int move_range_min = 300;
  int lines_pruned = 0;
  bool score_saved = 0;

  while(1) {
    switch(state) {
    case INIT:               // Initialize the game, only run one time 
      initscr();
      start_color();
      for(int color = 1; color < 8; color++) {
	init_pair(color,color,color);
      }
      nodelay(stdscr,TRUE);  // Do not wait for characters using getch.  
      noecho();              // Do not echo input characters 
      getmaxyx(stdscr,y,x);  // Get the screen dimensions 
      w = init_well(((x/2)-(WELL_WIDTH/2)),1,WELL_WIDTH,WELL_HEIGHT);
      draw_well(w);
      srand(time(NULL));     // Seed the random number generator with the time. Used in create tet.
      //start the game off with the user entering a name
      state = NAME;
      break;
    case NAME:
      mvprintw(5,5,"Enter your name:");
      mvprintw(7,7,"%s",name);
      if((arrow = read_escape(&c)) != NOCHAR) {
	if(c == '\n' || i >= 20) {
	state = ADD_PIECE;
	mvprintw(5,5,"                ");
	mvprintw(7,7,"                    ");
	break;
	}
	  name[i] = c;
	  i++;
      }
      break;
    case ADD_PIECE:          // Add a new piece to the game
      mvprintw(7,w->upper_left_x+30,"Next tetromino:");
      move_timeout = move_range_min + (rand() % 250);
      lines_pruned = prune_well(w);
      score = compute_score(score, lines_pruned);

      if(score % 5 == 0 && lines_pruned && level <= 5) {
	level++;
	move_range_min -= 50;
	if (move_range_min <= 0) move_range_min = 30;
	change_level();
      }
      
      if (next) {
	undisplay_tetromino(next);	
	destroy_tetromino(current);
	current = next;
	current->upper_left_x = w->upper_left_x+w->width/2;
	current->upper_left_y = w->upper_left_y;
	next = create_tetromino ((w->upper_left_x+31), w->upper_left_y+8);
      }
      else {
	current = create_tetromino ((w->upper_left_x+(w->width/2)), w->upper_left_y);
	next = create_tetromino ((w->upper_left_x+31), w->upper_left_y+8);
      }

      if (check_collision(current) == COLLIDE) {
	lives--;
	move_counter = 0;
	row_to_clear = 1;
	state = LIFE_LOST;
	if(lives == 0) {
	  state = GAME_OVER;
	  mvprintw(2, w->upper_left_x+30, name);
	  mvprintw(3, w->upper_left_x+30, "Lives: %i"      , lives);
	  mvprintw(4, w->upper_left_x+30, "Final Score: %i      ", score);	
	  mvprintw(7, w->upper_left_x+30,"                  ");
	  mvprintw(10,10,"                        ");
	  move_timeout = 10*BASE_FALL_RATE;
	}      
      } else {
	state = MOVE_PIECE;
	display_tetromino(next);
	mvprintw(2, w->upper_left_x+30, name);
	mvprintw(3, w->upper_left_x+30, "Lives: %i     ", lives);
	mvprintw(4, w->upper_left_x+30, "Current Score: %i     ", score);
	mvprintw(6, w->upper_left_x+30, "LEVEL: %i    ", level);
	mvprintw(1, w->upper_left_x-35, "Highscores:     ");
	for(int i = 0; i < 5; i++) {
	  mvprintw(2 + i, w->upper_left_x-35, "%i)%s - score:%i",(i+1),print_name(i), print_score(i));
	}
	display_tetromino(current);
      }
             
      break;
    case LIFE_LOST:
      if(row_to_clear <= w->height){
	for(int i = 1;i < w->width; i++){
	  mvprintw(w->upper_left_y + w->height - row_to_clear, w->upper_left_x + i, " ");
	}
	row_to_clear++;
      }
      if(move_counter++ >= move_timeout) {
       state = ADD_PIECE;
       move_counter = 0;
      }
      break;
    case MOVE_PIECE:         // Move the current piece
      //undisplay_tetromino(current);
      //move_tet(current,current->upper_left_x, current->upper_left_y + 1);
      //display_tetromino(current);
      if ((arrow = read_escape(&c)) != NOCHAR) {
	switch (arrow) { 
	case UP: 
	  undisplay_tetromino(current);
	  rotate_ccw(current);
	  display_tetromino(current);
	  break;
	case DOWN:
	  undisplay_tetromino(current);
	  rotate_cw(current);
	  display_tetromino(current);
	  break;
	case LEFT:
	  undisplay_tetromino(current);
	  move_tet(current,current->upper_left_x - 1, current->upper_left_y);
	  display_tetromino(current);
	  break;
	case RIGHT:
	  undisplay_tetromino(current);
	  move_tet(current,current->upper_left_x + 1, current->upper_left_y);
	  display_tetromino(current);
	  break;
	case REGCHAR: 
	  if (c == 'q' || c == 'x') {
	    state = EXIT;
 	  } else if(c == ' ') {
	    move_timeout = DROP_RATE;
	  } else if (c == 'p'){
	    state = PAUSE;
	  }
	  break;
	}
      }
      if (move_counter++ >= move_timeout) {
	undisplay_tetromino(current);
	  
	if(move_tet(current, current->upper_left_x, current->upper_left_y + 1) != MOVE_OK) {
	  state = ADD_PIECE;
	}

	display_tetromino(current);
	move_counter = 0;
      }
      if(timer_m++ >= 1000) {
	timer_s++;
	timer_m = 0;
      }
      mvprintw(5, w->upper_left_x+30, "Game Timer: %i      ", timer_s);
      break;
    case PAUSE:
      mvprintw(10,10,"PAUSED");
      if ((arrow = read_escape(&c)) != NOCHAR) {
	if(c == 'p') {
	  mvprintw(10,10,"UNPAUSED");
	  state = MOVE_PIECE;
	}
      }
      break;
    case GAME_OVER:
      if(!score_saved) {
	save_score(name,score);
      for(int i = 0; i <= 5; i++) {
	  mvprintw(1 + i, w->upper_left_x-35, "                            ");
      }
      score_saved = 1;
      }
      if(row_to_clear <= w->height){
	for(int i = 0;i < w->width; i++){
	  mvprintw(w->upper_left_y + w->height - row_to_clear, w->upper_left_x + i, " ");
	}
	row_to_clear++;
      }
      undraw_well(w);
      mvprintw(10, w->upper_left_x+w->width/2,"GAME OVER!");
     if(move_counter++ >= move_timeout) {
       state = EXIT;
      }
      break;

    case EXIT:
      endwin();
      return(0);
      break;
    }
    refresh();
    nanosleep(&tim,&tim_ret);
  }
}

/* game.cpp ends here */
