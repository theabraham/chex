#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <panel.h>

#define true  1
#define false 0

// way to expand macros to strings, ghetto
#define STR_EXPAND(x) #x
#define TO_STR(x) STR_EXPAND(x)

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
