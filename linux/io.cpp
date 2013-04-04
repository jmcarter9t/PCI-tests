#include "io.h"



int kbhit(void)
{
  nodelay(stdscr, 1);
  int ch = getch();
  if (ch != ERR) {
    return ch;
  } else {
    return 0;
  }
}
