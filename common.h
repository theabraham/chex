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

// way to expand macros to strings, ghetto
#define STR_EXPAND(x) #x
#define TO_STR(x) STR_EXPAND(x)

// If state doesn't evaluate to true, jump out of the main-loop, print the error and 
// exit the program.
#define CHECK(state) do { \
        if (!(state)) { sprintf(error_line, "[%s:%i] ", __FILE__, __LINE__); longjmp(error_pos, 1); } \
    } while (false)

jmp_buf error_pos;
char error_line[64];

// Colors available through ncurses. Can be used as foreground or background colors.
enum colors {
    WHITE = 1, BLACK, RED, GREEN, BLUE, CYAN, YELLOW, MAGENTA, ORANGE
};

typedef enum {
    ASCII, HEX
} modes_t;

typedef enum {
    ESCAPE, REPLACE
} states_t;

#include "view.h"
#include "buffer.h"
#include "route.h"

#endif
