#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <panel.h>

#define true  1
#define false 0

enum colors {
    WHITE = 1, BLACK, RED, GREEN, BLUE, CYAN, YELLOW, MAGENTA, ORANGE
};

#include "display.h"
#include "route.h"

#endif
