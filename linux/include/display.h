#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include <sys/io.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#define PRINTF printf
#define TEXT_COLOR(x) init_pair(1, x, COLOR_BLACK )
#define GETC  getchar
#define PUTS  puts
#define GOTOXY(x,y)  move(x,y)

#define CLRSCR() clear(); refresh();

#define KBHIT()    1

/* Colors */
#define GREEN COLOR_GREEN
#define RED  COLOR_RED
#define LIGHTGRAY COLOR_WHITE


#endif
