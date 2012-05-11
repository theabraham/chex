#ifndef DISPLAY_H
#define DISPLAY_H

// Create any needed ncurses windows and panels with sizes determined by how many 
// bytes of information each window will show.
void view_init(int bpaddr, int bpline, int bpseg);

// Free any memory used by the view.
void view_free();

// Clear the view's windows.
void view_clear();

// Draw the view.
void view_display(unsigned char *buffer, size_t size, size_t top, size_t bottom, int bpaddr, int bpline, int bpseg);

// Draw the message bar.
void view_msg(char *filename, modes_t mode, states_t state, size_t index, size_t size);

// Draw the cursor at the given `line` and `col`, within the `mode` panel.
void view_cursor(int line, int col, modes_t mode);

// Update the view.
void view_update();

#endif
