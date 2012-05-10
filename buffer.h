#ifndef BUFFER_T
#define BUFFER_T

// Open the named file and initialize the buffer with its contents.
void buf_init(char *filename);

// Free memory used by the buffer.
void buf_free();

// Write the buffer's state to the original file.
void buf_write();

// Display the buffer's current state.
void buf_draw();

// Move the buffer's index by (+/-) 1 line or column.
void buf_move(int line, int col);

// Get the buffer's editing mode.
modes_t buf_getmode();

// Set the buffer's editing mode.
void buf_setmode(modes_t mode);

// Gets the buffer's state.
states_t buf_getstate();

// Sets the buffer's state.
void buf_setstate(states_t state);

// Replace the character at the current buffer index, and move the index forward
// one. Hex characters will be converted to their represented values. 
void buf_repc(char ch);

void buf_begline();

void buf_endline();

void buf_nextseg();

void buf_prevseg();

#endif
