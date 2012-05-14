#include <signal.h>
#include "common.h"
#include "route.h"

#define CTRL(ch) ((ch) & 0x1F)

static void suspend()
{
    kill(getpid(), SIGTSTP);
}

static void toggle_mode()
{
    if (buf.mode == HEX) {
        buf.mode = ASCII;
        buf.nybble = false;
    } else {
        buf.mode = HEX;
    }
}

static void set_state(states_t state)
{
    buf.state = state
}

static void move_cursor(int line, int col)
{
    buf_mvline(line);
    buf_mvcol(col);
}

static void replace_char(char ch)
{
    bool is_backspace = (ch == KEY_BACKSPACE || ch == KEY_DC || ch == 127);
    if (is_backspace) {
        int orig;
        fseek(buf.fp, (buf.index == 0 ? 0 : buf.index - 1), SEEK_SET);
        orig = fgetc(buf.fp);
        mvcol(-1);
        setchar(orig);
     } else {
        setchar(ch);
        mvcol(+1);
    }
}

static void goto_line_beg()
{
    setindex(buf.index - (buf.index % buf.bpline), false);
}

static void goto_line_end()
{
    int index = (buf.index - (buf.index % buf.bpline)) + (buf.bpline - 1);
    if (index >= buf.size)
        setindex(buf.size - 1, true);
    else
        setindex(index, true);
}

static void goto_grp_next()
{
    setindex(buf.index + (buf.bpseg - (buf.index % buf.bpseg)), false);
}

static void goto_grp_prev()
{
    if (buf.index % buf.bpseg) {
        // go to beginning of current segment
        setindex(buf.index - (buf.index % buf.bpseg), false);
    } else {
        // go to beginning of previous segment
        setindex(buf.index - buf.bpseg, false);
    }
}

static void goto_buffer_beg()
{
    setindex(0, false);
}

static void goto_buffer_end()
{
    setindex(buf.size - (buf.size % buf.bpline), false);
}

static void goto_half_next()
{
    bool inbounds = setindex(buf.index + ((TMP_LINES / 2) * buf.bpline), buf.nybble);
    if (!inbounds)
        buf_end();
}

static void goto_half_prev()
{
    int inbounds = setindex(buf.index - ((TMP_LINES / 2) * buf.bpline), buf.nybble);
    if (!inbounds)
        buf_beg();
}

bool route(int ch)
{
    /* Keys available in both ESCAPE and REPLACE modes. */
    switch (ch) {
        case CTRL('c'):
        case CTRL('q'):   return false; break;
        case KEY_SUSPEND:
        case CTRL('z'):   suspend(); break;
        case CTRL('w'):   buf_write(); break;
        case CTRL('r'):   buf_revert(); break;
        case CTRL('['):   set_state(ESCAPE); break;
        case KEY_LEFT:    move_cursor(0, -1); break;         
        case KEY_UP:      move_cursor(-1, 0); break;        
        case KEY_RIGHT:   move_cursor(0, +1); break;         
        case KEY_DOWN:    move_cursor(+1, 0); break;        
        case '\t':        toggle_mode(); break;    
        default:
            if (buf.state == REPLACE) {
                replace_char(ch);
                return true;
            }
            break;
    }

    /* Keys available in only ESCAPE mode. */
    switch (ch) {
        case 'R': set_state(REPLACE); break;
        case 'h': move_cursor(0, -1); break; 
        case 'k': move_cursor(-1, 0); break;
        case 'l': move_cursor(0, +1); break; 
        case 'j': move_cursor(+1, 0); break;
        case 'w': goto_grp_next(); break;
        case 'b': goto_grp_prev(); break;
        case 'g': goto_buffer_beg(); break;
        case 'G': goto_buffer_end(); break;
        case '^': goto_line_beg(); break;
        case '$': goto_line_end(); break;
        case 'd': goto_half_next(); break;
        case 'u': goto_half_prev(); break;
        //case 'e': buf_mvedge(-1); break;
        //case 'y': buf_mvedge(+1); break;
        case '?': break;
        default:  break;                                    
    }

    return true;
}

