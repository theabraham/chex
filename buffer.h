#ifndef BUFFER_T
#define BUFFER_T

typedef enum modes_t {
    ASCII, HEX
};

typedef enum states_t {
    ESCAPE, REPLACE
};

void buf_init(char *filename);

void buf_free();

void buf_draw();




void buf_setpos();

void buf_nudge(int dist);

int buf_getpos();

void buf_setch(char ch);

int buf_getch();

// save buffer to file
// revert character at pos to original file pos
// revert buffer to original file
// error checking, permissions checking

#endif
