#ifndef DISPLAY_H
#define DISPLAY_H

// Initialize the buffer, some default values, and windows/panels for the view.
void view_init();

// Free any memory used by the view and the buffer.
void view_free();

// Draw the view.
void view_draw(char *filename, char *buffer, size_t size, size_t index, modes_t mode, states_t, state)

#endif
