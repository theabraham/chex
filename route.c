#include <signal.h>
#include "common.h"
#include "route.h"

#define CTRL(ch) ((ch)&0x1F)

static void suspend()
{
    kill(getpid(), SIGTSTP);
}

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
    // escape and replace mode keys
    switch (ch) {
        case CTRL('c'):
        case CTRL('q'): return false; break;
        case KEY_SUSPEND:
        case CTRL('z'): suspend(); break;
        case CTRL('w'): return false; break;
        case CTRL('['): buf_setstate(ESCAPE); break;
        case KEY_LEFT:  buf_move(0, -1); break;         
        case KEY_UP:    buf_move(-1, 0); break;        
        case KEY_RIGHT: buf_move(0, +1); break;         
        case KEY_DOWN:  buf_move(+1, 0); break;        
        case '\t':      toggle(); break;    
    }

    if (buf_getstate() == REPLACE) {
        buf_repc(ch);
        return true;
    }

    // escape mode keys
    switch (ch) {
        case 'h':       buf_move(0, -1); break; 
        case 'k':       buf_move(-1, 0); break;
        case 'l':       buf_move(0, +1); break; 
        case 'j':       buf_move(+1, 0); break;
        case 'g':       break;
        case 'G':       break;
        case '^':       break;
        case '$':       break;
        case CTRL('u'): break;
        case CTRL('d'): break;
        case 'R':       buf_setstate(REPLACE); break;
        case '?':       break;
        default: break;                                    
    }

    return true;
}

