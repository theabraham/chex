#include "common.h"
#include "display.h"

#define BUFFER_MLEN 1024

typedef struct buffer_t {
    char *mem;          // dynamic block of memory
    int clen;           // current length
    int mlen;           // max length
} buffer_t;

static struct {
    char *fname;        // filename
    FILE *fp;           // file-pointer
    buffer_t buffer;    // buffer for file contents
    WINDOW *win;
} disp;

void dispinit(char *fname)
{
    disp.fname = fname;
    disp.fp = fopen(fname, "r+");
    disp.buffer.clen = 0;
    disp.buffer.mlen = BUFFER_MLEN;
    disp.buffer.mem = malloc(disp.buffer.mlen*sizeof(char));
    disp.win = newwin(LINES, COLS, 0, 0);

    // read file into buffer
    int ch;
    while ((ch = fgetc(disp.fp)) != EOF) {
        disp.buffer.mem[disp.buffer.clen++] = ch;
    }
}

void dispfree()
{
    fclose(disp.fp);
    delwin(disp.win);
    free(disp.buffer.mem);
}

void dispdraw()
{
    clear();
    int i, row = 1;
    printw("%07x: ", (row++)*16);
    for (i=0; i<disp.buffer.clen; i++) {
        printw("%02x", disp.buffer.mem[i]);
        if ((i+1)%16 == 0) {
            printw("\n%07x: ", (row++)*16);
        } else if ((i+1)%2 == 0) {
            printw(" ");
        }
    }
    refresh();
}
