#include "common.h"
#include "route.h"

bool route(int ch)
{
    switch (ch) {
        case 'q':                   return false; break;    // quit
        case 'h': case KEY_LEFT:    mvcol(-1); break;       // move left
        case 'j': case KEY_DOWN:    mvline(+1); break;      // move down
        case 'k': case KEY_UP:      mvline(-1); break;      // move up
        case 'l': case KEY_RIGHT:   mvcol(+1); break;       // move right
        default: break;                                     // unmapped key
    }
    
    return true;
}

