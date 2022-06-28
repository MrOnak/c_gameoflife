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
#include <curses.h>

#define COLS 50
#define ROWS 20

bool grids[2][ROWS][COLS] = { false };
bool paused = false;
uint8_t gridIndex = 0;
uint32_t delay = 100000;

void printActiveGrid() {
  uint8_t x,y;

  attron(COLOR_PAIR(1));
  printw(" Conway's Game of Life (delay: %dms)\n", delay / 1000);
  attroff(COLOR_PAIR(1));

  attron(COLOR_PAIR(3));
  printw("+%*s+\n", COLS, "");

  for (y = 0; y < ROWS; y++) {
    printw("|");
    attroff(COLOR_PAIR(3));

    for (x = 0; x < COLS; x++) {
      if (grids[gridIndex][y][x] == true) {
        attron(COLOR_PAIR(2));
        printw("*");
        attroff(COLOR_PAIR(2));
      } else {
        attron(COLOR_PAIR(1));
        printw(" ");
        attroff(COLOR_PAIR(1));
      }
    }

    attron(COLOR_PAIR(3));
    printw("|\n");
  }

  printw("+%*s+\n", COLS, "");
  attroff(COLOR_PAIR(3));
  attron(COLOR_PAIR(1));
}

void initGrid() {
  uint8_t x,y;

  srand(time(NULL));

  for (y = 0; y < ROWS; y++) {
    for (x=0; x < COLS; x++) {
      grids[gridIndex][y][x] = rand() & 1;
    }
  }
}

void calculateStep() {
  uint8_t x, y, o, nx, ny;
  uint8_t active;

  // shorthands for current and next grid index
  uint8_t i = gridIndex;
  uint8_t ni = (i + 1) % 2;

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


int main(int argc, char *argv[]) {
  initscr();
  cbreak();
  noecho();
  timeout(1);

  if (has_colors() == false) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_GREEN);
  init_pair(3, COLOR_WHITE, COLOR_WHITE);

  initGrid();

  while (true) {
    // print active grid
    clear();
    printActiveGrid();
    refresh();

    if (!paused) {
      // update new grid
      calculateStep();
    }

    // pause
    usleep(delay);

    // handle user input
    int c = getch();

    switch(c) {
      case 43: // "+" - double delay
        delay *= 2;
        break;
      case 45: // "-" - reduce delay by half
        if (delay > 50000) {delay /= 2;}
        break;
      case 113: // "q" - quit
        endwin();
        exit(EXIT_SUCCESS);
        break;
      case 110: // "n" - new grid
        initGrid();
        break;
      case 112: // "p" - pause
        paused = !paused;
        break;
      default:
    }
  }

  endwin();
  return EXIT_SUCCESS;
}
