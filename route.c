#include "common.h"
#include "route.h"

#define CTRL(ch) ((ch)&0x1F)

bool route(int ch)
{
    // special keys
    switch (ch) {
        case CTRL('c'):
        case CTRL('q'): return false; break;
        case CTRL('w'): return false; break;
        case CTRL('['): disp_setmode(ESCAPE); break;
        case KEY_LEFT:  disp_move(0, -1); break;         
        case KEY_UP:    disp_move(-1, 0); break;        
        case KEY_RIGHT: disp_move(0, +1); break;         
        case KEY_DOWN:  disp_move(+1, 0); break;        
    }

    if (disp_getmode() == REPLACE) {
        disp_replace(ch);
        return true;
    }

    // escape-mode keys
    switch (ch) {
        // movement keys
        case 'h':       disp_move(0, -1); break; 
        case 'k':       disp_move(-1, 0); break;
        case 'l':       disp_move(0, +1); break; 
        case 'j':       disp_move(+1, 0); break;
        case '\t':      disp_toggle(); break;    
        case 'g':       break;
        case 'G':       break;
        case '^':       break;
        case '$':       break;
        case CTRL('u'): break;
        case CTRL('d'): break;

        // miscellaneous keys
        case 'R':       disp_setmode(REPLACE); break;
        case '?':       break;

        // unmapped key
        default: break;                                    
    }

    // TODO: support for ^C, ^Z, etc because of 'raw()'.
    return true;
}

