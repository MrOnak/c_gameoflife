/*
 ===========================================================================================
 Conway's Game of Life
    an educational implementation

    Author: Dominique Stender
 ===========================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>

#define COLS 50 
#define ROWS 20

bool grids[2][ROWS][COLS] = { false };
uint8_t gridIndex = 0;

void printActiveGrid() {
  uint8_t x,y;

  printf("\033[H"); // move to top-left of screen

  printf(" Conway's Game of Life\n");

  printf(" +");
  for (x = 0; x < COLS; x++) {
    printf("-");
  }
  printf("+\n");

  for (y = 0; y < ROWS; y++) {
    printf(" |");

    for (x = 0; x < COLS; x++) {
      if (grids[gridIndex][y][x] == true) {
        printf("*");
      } else {
        printf(" ");
      }
    }
    
    printf("|\n");
  }

  printf(" +");
  for (x = 0; x < COLS; x++) {
    printf("-");
  }
  printf("+\n");
}

void initGrid() {
  uint8_t x,y;

  srand(time(NULL));
  
  for (y = 0; y < ROWS; y++) {
    for (x=0; x < COLS; x++) {
      grids[gridIndex][y][x] = rand() & 1;
    }
  }
  
  /*
  // single glider for testing
  grids[gridIndex][0][1] = true;
  grids[gridIndex][1][2] = true;
  grids[gridIndex][2][0] = true;
  grids[gridIndex][2][1] = true;
  grids[gridIndex][2][2] = true;
  */
}

void calculateStep() {
  uint8_t x, y, o, nx, ny;
  uint8_t active;

  // shorthands for current and next grid index
  uint8_t i = gridIndex;
  uint8_t ni = (i + 1) % 2;

  /*
  printf("current grid index: %d\n", i);
  printf("new grid index    : %d\n", ni);
  */

  // neighbour offsets
  int8_t ox[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  int8_t oy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

  // calculate new grid
  for (y = 0; y < ROWS; y++) {
    for (x = 0; x < COLS; x++) {
      active = 0; // active cell counter;
       
      for (o = 0; o < 8; o++) {
        // neighbour coordinates
        nx = (x + ox[o] + COLS) % COLS;
        ny = (y + oy[o] + ROWS) % ROWS;
        
        // printf("%d/%d + %d/%d = %d/%d\n", x, y, ox[o], oy[o], nx, ny);

        if (grids[i][ny][nx] == true) {
          active++;
        }
      }

      if (grids[i][y][x] == true) {
        switch (active) {
          case 2:
          case 3:
            grids[ni][y][x] = true;
            break;
          default:
            grids[ni][y][x] = false;
        }
      } else {
        switch (active) {
          case 3: 
            grids[ni][y][x] = true;
            break;
          default:
            grids[ni][y][x] = false;
        }
      }
    }
  }

  // update grid index
  gridIndex = ni;
}

void setBufferedInput(bool enable) {
  static bool enabled = true;
  static struct termios old;
  struct termios new;

  if (enable && !enabled) {
    // restore the former settings
    tcsetattr(STDIN_FILENO,TCSANOW,&old);
    // set the new state
    enabled = true;
  } else if (!enable && enabled) {
    // get the terminal settings for standard input
    tcgetattr(STDIN_FILENO,&new);
    // we want to keep the old setting to restore them at the end
    old = new;
    // disable canonical mode (buffered i/o) and local echo
    new.c_lflag &=(~ICANON & ~ECHO);
    // set the new settings immediately
    tcsetattr(STDIN_FILENO,TCSANOW,&new);
    // set the new state
    enabled = false;
  }
}

void signal_callback_handler(int signum) {
  printf("         TERMINATED         \n");
  setBufferedInput(true);
  printf("\033[?25h\033[m");
  exit(signum);
}

int main(int argc, char *argv[]) {
  // register signal handler for when ctrl-c is pressed
  signal(SIGINT, signal_callback_handler);

  initGrid();

  printf("\033[?25l\033[2J");

  while (true) {
    usleep(100000);
    printActiveGrid();
    calculateStep();
    
    //printf("gridIndex: %d\n", gridIndex);
    printf("\n\n");
  }

  return EXIT_SUCCESS;
}
