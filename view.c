#include "common.h"

// Given `b` bytes, figure out how many columns wide a window is.
#define ADDR_COLS(b)        ((b)+1)         // takes into account ':' character
#define HEX_COLS(b, seg)    ((b*2)+(b/seg)) // hex segments plus spaces inbetween
#define ASCII_COLS(b)       (b)      
#define EDITOR_LINES        (LINES - 1)

#define LINES_IN_BUFFER (buf.size / view.bpline)

/* The INDEX's current column within a line. */
#define INDEX_COL (buf.index % view.bpline)

/* Cursor's LINE and COL position relative to the buffer's INDEX. In HEX mode,
   the COL accounts for spaces inbetween hex groups and the current NYBBLE. */
#define CURSOR_LINE ((buf.index / view.bpline) - view.edge)
#define CURSOR_COL (buf.mode == HEX ? ((INDEX_COL * 2) + (INDEX_COL / view.bpgrp) + buf.nybble) \
                                    : (buf.index % view.bpline))

/* Moves the EDGE by LINES. If the new EDGE would be out of bounds, set it to
   either the first or last line in the buffer. */
static void edge_move(int lines)
{
    int edge = view.edge + lines;
    if (edge < 0) {
        view.edge = 0;
    } else if (edge > LINES_IN_BUFFER) {
        view.edge = LINES_IN_BUFFER;
    } else {
        view.edge = edge;
    }
}

/* Return the INDEX that marks the beginning of the currently visible buffer. */
static int edge_beg()
{
    return view.edge * view.bpline;
}

/* Returns the INDEX that marks the end of the currently visible buffer. */
static int edge_end()
{
    int end = edge_beg() + (view.bpline * EDITOR_LINES) - 1;
    if (end >= buf.size)
        end = buf.size - 1;
    return end;
}

/* Adjust the EDGE so the cursor stays within its boundaries. */
static void edge_adjust()
{
    while (buf.index < edge_beg()) edge_move(-1);
    while (buf.index > edge_end()) edge_move(+1);
}

void view_init(int bpaddr, int bpline, int bpgrp)
{
    view.edge = 0;
    view.bpaddr = bpaddr;
    view.bpline = bpline;
    view.bpgrp = bpgrp;
    view._addrwin = newwin(LINES-1, ADDR_COLS(bpaddr), 0, 0);
    view._hexwin = newwin(LINES-1, HEX_COLS(bpline, bpgrp), 0, ADDR_COLS(bpaddr)+1);
    view._asciiwin = newwin(LINES-1, ASCII_COLS(bpline), 0, ADDR_COLS(bpaddr)+HEX_COLS(bpline, bpgrp)+1);
    view._msgwin = newwin(1, COLS, LINES-1, 0);
    view._addrpan = new_panel(view._addrwin);
    view._hexpan = new_panel(view._hexwin);
    view._asciipan = new_panel(view._asciiwin);
    view._msgpan = new_panel(view._msgwin);
}

void view_free()
{
    delwin(view._addrwin);
    delwin(view._hexwin);
    delwin(view._asciiwin);
    delwin(view._msgwin);
    del_panel(view._addrpan);
    del_panel(view._hexpan);
    del_panel(view._asciipan);
    del_panel(view._msgpan);
}

void view_clear()
{
    wclear(stdscr);
    wclear(view._addrwin);
    wclear(view._hexwin);
    wclear(view._asciiwin);
    wclear(view._msgwin);
    show_panel(view._addrpan);
    show_panel(view._hexpan);
    show_panel(view._asciipan);
    show_panel(view._msgpan);
}

void view_status()
{
    char *mode_str = (buf.mode == HEX ? "HEX" : "ASCII");
    char *state_str = (buf.state == ESCAPE ? "" : "REPLACE");
    wprintw(view._msgwin, "\"%s\" %-5s %-7s [%i/%i]", buf.filename, mode_str, state_str, buf.index, buf.size);
}

void view_cursor()
{
    PANEL *curpan = (buf.mode == HEX ? view._hexpan : view._asciipan);
    wmove(panel_window(curpan), CURSOR_LINE, CURSOR_COL);
    top_panel(curpan);
}

void view_help()
{
    box(stdscr, false, false);
    wattrset(stdscr, A_BOLD);
    mvwprintw(stdscr, (LINES/2)-4, (COLS/2)-3, "CONTROLS");
    wattrset(stdscr, A_NORMAL);
    mvwprintw(stdscr, (LINES/2)-3, (COLS/2)-6, "right - right/h");
    mvwprintw(stdscr, (LINES/2)-2, (COLS/2)-5, "left - left/l");
    mvwprintw(stdscr, (LINES/2)-1, (COLS/2)-7, "rotate - up/k");
    mvwprintw(stdscr, (LINES/2)+0, (COLS/2)-5, "down - down/j");
    mvwprintw(stdscr, (LINES/2)+1, (COLS/2)-5, "drop - space");
    mvwprintw(stdscr, (LINES/2)+2, (COLS/2)-6, "pause - p");
    mvwprintw(stdscr, (LINES/2)+3, (COLS/2)-9, "controls - ?");
    mvwprintw(stdscr, (LINES/2)+4, (COLS/2)-5, "quit - q");
    hide_panel(view._addrpan);
    hide_panel(view._hexpan);
    hide_panel(view._asciipan);
}

void view_update()
{
    update_panels();
    doupdate();
}

void view_display()
{
    view_clear();

    if (view.help) {
        view_help();
        return;
    }

    int index, ch;
    bool use_space, is_ascii, on_current_line, has_changed;
    edge_adjust();
    for (index = edge_beg(); index <= edge_end(); index++) {
        ch = buf.mem[index];
        use_space = (((index + 1) % view.bpgrp) == 0);
        is_ascii = (ch >= ' ' && ch < 127);
        on_current_line = (((index / view.bpline) - view.edge) == CURSOR_LINE);
        fseek(buf.fp, index, SEEK_SET);
        has_changed = (ch != fgetc(buf.fp));

        if (on_current_line) {
            wattron(view._addrwin, A_BOLD);
            wattron(view._hexwin, A_BOLD);
            wattron(view._asciiwin, A_BOLD);
        }

        /* Draw address panel. */
        if (index % view.bpline == 0) {
            wprintw(view._addrwin, "%8x:", index);
        }

        /* Draw hex panel. */
        if (buf.mode == HEX) wattron(view._hexwin, COLOR_PAIR(WHITE));
        if (has_changed) wattron(view._hexwin, COLOR_PAIR(YELLOW));
        wprintw(view._hexwin, "%02x%s", ch, (use_space ? " " : ""));
        if (has_changed) wattroff(view._hexwin, COLOR_PAIR(YELLOW));
        if (buf.mode == HEX) wattroff(view._hexwin, COLOR_PAIR(WHITE));

        /* Draw ascii panel. */
        if (buf.mode == ASCII) wattron(view._asciiwin, COLOR_PAIR(WHITE));
        if (has_changed) wattron(view._asciiwin, COLOR_PAIR(YELLOW));
        waddch(view._asciiwin, (is_ascii ? ch : '.'));
        if (has_changed) wattroff(view._asciiwin, COLOR_PAIR(YELLOW));
        if (buf.mode == ASCII) wattroff(view._asciiwin, COLOR_PAIR(WHITE));

        if (on_current_line) {
            wattroff(view._addrwin, A_BOLD);
            wattroff(view._hexwin, A_BOLD);
            wattroff(view._asciiwin, A_BOLD);
        }
    }

    view_status();
    view_cursor();
    view_update();
}

