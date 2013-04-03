#ifndef _LINUX_IO_H
#define _LINUX_IO_H

#include <curses.h>
#include <ncurses.h>
#include <sys/io.h>
#include <unistd.h>
#include <ctype.h>


#define PRINTF printf
#define READ_CONFIGURATION_WORD(w,x,y,z) printf("Doing something\n");
#define TEXT_COLOR(x) init_pair(1, x, COLOR_BLACK )
#define GETC  getchar
#define PUTS  puts
#define OUTPORT outb
#define OUTPORTB outb
#define INPORT inb_p
#define INPORTB inb
#define GOTOXY(x,y)  move(x,y)
#define STRCMP   strcmp
#define KBHIT()    1
#define GETCH   getchar
#define DELAY  usleep
#define clrscr() clear(); refresh();

#define initirq(x,y)   printf("Implement 'initirq'")
#define restoreirq(x) printf("Implement 'restoreirq' ")

#define relay_write_test(x,y) printf("Implement 'relay_write_test'");
#define relay_step_test(x,y) printf("Implement 'relay_step_test'");
#define soft_filter_test(x,y) printf("Implement 'soft_filter_test'");
#define manual_filter_test(x,y) printf("Implement 'manual_filter_test'");
#define interrupt_test(x,y,z) printf("Implement 'interrupt_test'");
#define iiro16_relay_write_test(x,y,z) printf("Implement 'iiro16_relay_write_test'");
#define iiro16_relay_step_test(x,y,z)  printf("Implement 'iiro16_relay_step_test'");
#define iiro16_soft_filter_test(x,y,z) printf("Implement 'iiro16_soft_filter_test'");
#define iiro16_interrupt_test(w,x,y,z) printf("Implement 'iiro16_interrupt_test'");

/* PCI related */
#define pci_bios_present(x,y,z) printf("Implement 'pci_bios_present'")
#define find_pci_device(a,b,c,d,e) printf("Implement 'find_pci_device'")
#define read_configuration_byte(a,b,c,d) printf("Implement 'read_configuration_byte'")

/* Colors */
#define GREEN COLOR_GREEN
#define RED  COLOR_RED
#define LIGHTGRAY COLOR_WHITE

#endif
