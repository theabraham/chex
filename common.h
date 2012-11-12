#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <ncurses.h>
#include <panel.h>

#define true  1
#define false 0

/* Display a formatted string to the far right of the screen; useful for debugging. */
#define debug(y, fmt, ...) mvwprintw(stdscr, y, 100, fmt, ##__VA_ARGS__)

/* If STATE doesn't evaluate to true, jump out of the main function's 
   event-loop, print the error, and exit the program. */
#define CHECK(state) do { \
        if (!(state)) { sprintf(error_line, "[%s:%i] ", __FILE__, __LINE__); longjmp(error_pos, 1); } \
    } while (false)


/* In the event of an error, this global string will store an error's message. */
char error_line[64];

/* In the event of an error, this global buffer will store an error's position. */ 
jmp_buf error_pos;

/* Colors available through ncurses. Can be used as foreground or background colors. */
enum colors {
    WHITE = 1, BLACK, RED, GREEN, BLUE, CYAN, YELLOW, MAGENTA, ORANGE
};

/* Differentiate between HEX and ASCII editing modes. */
typedef enum {
    ASCII, HEX
} modes_t;

/* Differentiate between ESCAPE and REPLACE editing states. */
typedef enum {
    ESCAPE, REPLACE
} states_t;

#include "buffer.h"
#include "view.h"
#include "route.h"

#endif
