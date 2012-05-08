#include "common.h"
#include "view.h"

#define ADDR_BYTES          8        // default address byte size
#define BYTES_PER_LINE      16       // default `bpline`
#define BYTES_PER_SEGMENT   4        // default `bpseg`

// Given `b` bytes, figure out how many columns wide a window is.
#define ADDR_COLS(b)        ((b)+1)  // takes into account ':' character
#define HEX_COLS(b)         ((b)*3)  // hex characters plus space inbetween
#define ASCII_COLS(b)       (b)      

static struct {
    int bpline;         // bytes of data to show per line (for ascii/hex)
    int bpseg;          // bytes of data to show per segment (for hex)
    WINDOW *addrwin;
    WINDOW *hexwin;     
    WINDOW *asciiwin;  
    WINDOW *msgwin;   
    PANEL *addrpan;
    PANEL *hexpan;   
    PANEL *asciipan;
    PANEL *msgpan; 
} view;                 

// Get the cursor's line position based on the buffer's position.
int getline(size_t index, modes_t mode)
{
    return index/view.bpline;
}

// Get the cursor's column position based on the buffer's position.
int getcol(size_t index, modes_t mode)
{
    if (mode == HEX) {
        return 0;
        //int half = (buf_ishalf() ? 1 : 0);
        //return buf_getpos()+half+(buf_getpos()/view.bpseg);
    } else {
        return (index%view.bpline);
    }
}

void view_init()
{
    view.bpline = BYTES_PER_LINE;
    view.bpseg = BYTES_PER_SEGMENT;
    view.addrwin = newwin(LINES-1, ADDR_COLS(ADDR_BYTES), 0, 0);
    view.hexwin = newwin(LINES-1, HEX_COLS(view.bpline), 0, ADDR_COLS(ADDR_BYTES)+1);
    view.asciiwin = newwin(LINES-1, ASCII_COLS(view.bpline), 0, ADDR_COLS(ADDR_BYTES)+HEX_COLS(view.bpln)+1);
    view.msgwin = newwin(1, COLS, LINES-1, 0);
    view.addrpan = new_panel(view.addrwin);
    view.hexpan = new_panel(view.hexwin);
    view.asciipan = new_panel(view.asciiwin);
    view.msgpan = new_panel(view.msgwin);
    view.curpan = view.hexpan;
}

void view_free()
{
    delwin(view.addrwin);
    delwin(view.hexwin);
    delwin(view.asciiwin);
    delwin(view.msgwin);
    del_panel(view.addrpan);
    del_panel(view.hexpan);
    del_panel(view.asciipan);
    del_panel(view.msgpan);
}

void view_draw(char *filename, char *buffer, size_t size, size_t index, modes_t mode, states_t, state)
{
    wclear(view.addrwin);
    wclear(view.hexwin);
    wclear(view.asciiwin);
    wclear(view.msgwin);
    show_panel(view.addrpan);
    show_panel(view.hexpan);
    show_panel(view.asciipan);
    show_panel(view.msgpan);

    int i, ch;
    for (i=0; i<size; i++) {
        ch = buffer[i];

        // draw address
        if (i%16 == 0)
            wprintw(view.addrwin, "%0" TO_STR(ADDR_BYTES) "x:", i);

        // draw hex
        wprintw(view.hexwin, "%02x ", ch);

        // draw ascii 
        // TODO: print special characters (or color) for NL, CR, etc.
        if (ch >= ' ' && ch < 127) {
            waddch(view.asciiwin, ch);
        } else {
            waddch(view.asciiwin, '.');
        }
    }

    // place cursor
    PANEL *curpan = (mode == HEX ? view.hexpan : view.asciipan);
    wmove(panel_window(curpan), getline(index, mode), getcol(index, mode));
    top_panel(curpan);

    // draw message bar 
    // TODO: will print errors and status like vim
    wprintw(view.msgwin, "\"%s\" %-5s %-7s", filename, (mode == HEX ? "HEX" : "ASCII"), (state == ESCAPE ? "" : "REPLACE"));

    update_panels();
    doupdate();
}

//void disp_toggle()
//{
//    disp.curpan = (disp.curpan == disp.hexpan ? disp.asciipan : disp.hexpan);
//}
//
//void disp_move(int line, int col)
//{
//    buf_nudge((line*disp.bpline)+col);
//}

//void disp_replace(int ch)
//{
//    if (disp.curpan == disp.hexpan) {
//        if (ishexnumber(ch)) {
//            char hex = { ch, '\0' };
//            ch = strtol(hex, NULL, 16);
//            buf_setch(ch, HALF);
//            buf_nudge(+1);
//        }
//    } else {
//        buf_setch(ch, WHOLE);
//        buf_nudge(+1);
//    }
//}

