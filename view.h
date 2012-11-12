#ifndef DISPLAY_H
#define DISPLAY_H

struct {
    int edge;           /* Line where the top edge of the visible buffer is. */
    int bpaddr;         /* Bytes to show per line in the address panel. */
    int bpline;         /* Bytes to show per line in the HEX and ASCII panels. */
    int bpgrp;          /* Bytes to show per each space delimited HEX group. */
    bool help;          /* Whether or not the help menu is currently active. */
    WINDOW *_addrwin;
    WINDOW *_hexwin;     
    WINDOW *_asciiwin;  
    WINDOW *_msgwin;   
    PANEL *_addrpan;
    PANEL *_hexpan;   
    PANEL *_asciipan;
    PANEL *_msgpan; 
} view;                 

/* Global help string describing available control keys. */
const char *controls;

/* Create the needed ncurses windows and panels with sizes determined by how many 
   bytes of information each window will show. */
void view_init(int bpaddr, int bpline, int bpgrp);

/* Free any memory used by the view. */
void view_free();

/* Clear the view's windows. */
void view_clear();

/* Draw the status bar. */
void view_status();

/* Display the cursor in the current editing panel. */
void view_cursor();

/* Update the ncurses panels after drawing the view. */
void view_update();

/* Draw the address, hex, and ascii panels to reflect the buffer's current state. */
void view_display();

#endif
