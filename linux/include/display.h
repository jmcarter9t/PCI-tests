#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include <sys/io.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "io.h"

#define CPRINTF(...) printw( __VA_ARGS__ ); refresh();
#define PRINTF printf
#define TEXT_COLOR(x) attron(COLOR_PAIR(x)); _color_display = x; 
#define GETC  getchar
#define GETCH getch
#define PUTS  puts
#define GOTOXY(x,y)  move(y,x)
#define WHEREY()  getcury(stdscr)
#define ENDWIN()  sleep(2); endwin();
#define ENDTEST() sleep(3); endwin();

#define RESETLINE(); (--stdscr->_cury);

#define CLRSCR() \
stdscr = initscr(); \
keypad(stdscr, TRUE);\
start_color(); \
init_pair( 0 , COLOR_BLACK     , COLOR_BLACK );\
init_pair( 1 , COLOR_RED       , COLOR_BLACK );\
init_pair( 2 , COLOR_GREEN     , COLOR_BLACK );\
init_pair( 3 , COLOR_YELLOW    , COLOR_BLACK );\
init_pair( 4 , COLOR_BLUE      , COLOR_BLACK );\
init_pair( 5 , COLOR_MAGENTA   , COLOR_BLACK );\
init_pair( 6 , COLOR_CYAN      , COLOR_BLACK );\
init_pair( 7 , COLOR_WHITE     , COLOR_BLACK );\
refresh();

#define KBHIT()  kbhit()

extern int _color_display;

/* Colors */

#define LIGHTGRAY COLOR_WHITE

#define RESET   COLOR_RESET
#define BLUE    COLOR_BLUE
#define CYAN    COLOR_CYAN
#define GREEN   COLOR_GREEN
#define MAGENTA COLOR_MAGENTA
#define NORMAL  COLOR_NORMAL
#define RED     COLOR_RED
#define WHITE   COLOR_WHITE
#define YELLOW  COLOR_YELLOW

#endif
