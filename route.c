#include "common.h"
#include "route.h"

#define CTRL(ch) ((ch)&0x1F)

static void toggle()
{
    if (buf_getmode() == HEX) {
        buf_setmode(ASCII);
    } else {
        buf_setmode(HEX);
    }
}

bool route(int ch)
{
    // special keys
    switch (ch) {
        case CTRL('c'):
        case CTRL('q'): return false; break;
        case CTRL('w'): return false; break;
        case CTRL('['): buf_setstate(ESCAPE); break;
        case KEY_LEFT:  buf_move(0, -1); break;         
        case KEY_UP:    buf_move(-1, 0); break;        
        case KEY_RIGHT: buf_move(0, +1); break;         
        case KEY_DOWN:  buf_move(+1, 0); break;        
    }

    if (buf_getstate() == REPLACE) {
        buf_repc(ch);
        return true;
    }

    // escape-mode keys
    switch (ch) {
        // movement keys
        case 'h':       buf_move(0, -1); break; 
        case 'k':       buf_move(-1, 0); break;
        case 'l':       buf_move(0, +1); break; 
        case 'j':       buf_move(+1, 0); break;
        case '\t':      toggle(); break;    
        case 'g':       break;
        case 'G':       break;
        case '^':       break;
        case '$':       break;
        case CTRL('u'): break;
        case CTRL('d'): break;

        // miscellaneous keys
        case 'R':       buf_setstate(REPLACE); break;
        case '?':       break;

        // unmapped key
        default: break;                                    
    }

    // TODO: support for ^C, ^Z, etc because of 'raw()'.
    return true;
}

