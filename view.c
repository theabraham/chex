#include "common.h"
#include "view.h"

// Given `b` bytes, figure out how many columns wide a window is.
#define ADDR_COLS(b)        ((b)+1)  // takes into account ':' character
#define HEX_COLS(b)         ((b)*3)  // hex characters plus space inbetween
#define ASCII_COLS(b)       (b)      

static struct {
    WINDOW *addrwin;
    WINDOW *hexwin;     
    WINDOW *asciiwin;  
    WINDOW *msgwin;   
    PANEL *addrpan;
    PANEL *hexpan;   
    PANEL *asciipan;
    PANEL *msgpan; 
} view;                 

void view_init(int bpaddr, int bpline, int bpseg)
{
    view.addrwin = newwin(LINES-1, ADDR_COLS(bpaddr), 0, 0);
    view.hexwin = newwin(LINES-1, HEX_COLS(bpline), 0, ADDR_COLS(bpaddr)+1);
    view.asciiwin = newwin(LINES-1, ASCII_COLS(bpline), 0, ADDR_COLS(bpaddr)+HEX_COLS(bpline)+1);
    view.msgwin = newwin(1, COLS, LINES-1, 0);
    view.addrpan = new_panel(view.addrwin);
    view.hexpan = new_panel(view.hexwin);
    view.asciipan = new_panel(view.asciiwin);
    view.msgpan = new_panel(view.msgwin);
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

void view_display(unsigned char *buffer, size_t size)
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
            wprintw(view.addrwin, "%08x:", i);
            //wprintw(view.addrwin, "%0" TO_STR(ADDR_BYTES) "x:", i);

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
}

void view_msg(char *filename, modes_t mode, states_t state, size_t index, size_t size)
{
    // TODO: will print errors and status like vim
    char *mode_str = (mode == HEX ? "HEX" : "ASCII");
    char *state_str = (state == ESCAPE ? "" : "REPLACE");
    wprintw(view.msgwin, "\"%s\" %-5s %-7s [%i/%i]", filename, mode_str, state_str, index, size);
}

void view_cursor(int line, int col, modes_t mode)
{
    PANEL *curpan = (mode == HEX ? view.hexpan : view.asciipan);
    wmove(panel_window(curpan), line, col);
    top_panel(curpan);
}

void view_update()
{
    update_panels();
    doupdate();
}

