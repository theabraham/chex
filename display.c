#include "common.h"
#include "display.h"

#define BUFFER_MLEN 1024
#define ADDR_COLS(b) ((b)+1)
#define HEX_COLS(b) ((b)*3)
#define ASCII_COLS(b) (b)

typedef struct {
    char *mem;          // dynamic block of memory
    int clen;           // current length
    int mlen;           // max length
} buffer_t;

static struct {
    char *fname;        // filename
    FILE *fp;           // file-pointer
    buffer_t buffer;    // buffer for file contents
    int line;           // cursor's line position (TODO break off into struct)
    int col;            // cursor's col position
    int addrbytes;      // how many address bytes to show
    int hexbytes;       // how many hex bytes to show
    WINDOW *addrwin;
    WINDOW *hexwin;
    WINDOW *asciiwin;
    WINDOW *msgwin;
    PANEL *addrpan;
    PANEL *hexpan;
    PANEL *asciipan;
    PANEL *msgpan;
} disp;

void dispinit(char *fname)
{
    disp.fname = fname;
    disp.fp = fopen(fname, "r+");
    disp.buffer.clen = 0;
    disp.buffer.mlen = BUFFER_MLEN;
    disp.buffer.mem = malloc(disp.buffer.mlen*sizeof(char));
    disp.line = 0;
    disp.col = 0;
    disp.addrbytes = 8; // TODO change magic number
    disp.hexbytes = 16; // TODO change magic number
    disp.addrwin = newwin(LINES, ADDR_COLS(disp.addrbytes), 0, 0);
    disp.hexwin = newwin(LINES, HEX_COLS(disp.hexbytes), 0, ADDR_COLS(disp.addrbytes)+1);
    disp.asciiwin = newwin(LINES, ASCII_COLS(disp.hexbytes), 0, ADDR_COLS(disp.addrbytes)+HEX_COLS(disp.hexbytes)+1);
    disp.msgwin = newwin(1, COLS, LINES-1, 0);
    disp.addrpan = new_panel(disp.addrwin);
    disp.hexpan = new_panel(disp.hexwin);
    disp.asciipan = new_panel(disp.asciiwin);
    disp.msgpan = new_panel(disp.msgwin);

    // read file into buffer
    int ch;
    while ((ch = fgetc(disp.fp)) != EOF) {
        disp.buffer.mem[disp.buffer.clen++] = ch;
    }
}

void dispfree()
{
    fclose(disp.fp);
    //delwin(disp.win); // TODO: free windows
    free(disp.buffer.mem);
}

void dispdraw()
{
    // clear display
    wclear(disp.addrwin);
    wclear(disp.hexwin);
    wclear(disp.asciiwin);
    wclear(disp.msgwin);
    show_panel(disp.addrpan);
    show_panel(disp.hexpan);
    show_panel(disp.asciipan);
    show_panel(disp.msgpan);

    // draw address
    int i;
    for (i=0; i<=disp.buffer.clen; i+=disp.hexbytes) {
        wprintw(disp.addrwin, "%08x:", i);
    }

    // draw hex
    for (i=0; i<disp.buffer.clen; i++) {
        wprintw(disp.hexwin, "%02x ", disp.buffer.mem[i]);
    }

    // draw ascii
    int ch;
    for (i=0; i<disp.buffer.clen; i++) {
        ch = disp.buffer.mem[i];
        if (ch >= ' ' && ch < 127) { // TODO: print special characters (or color) for NL, CR, etc.
            waddch(disp.asciiwin, ch);
        } else {
            waddch(disp.asciiwin, '.');
        }
    }

    // message bar (TODO: will print errors and status like vim)
    wprintw(disp.msgwin, "\"%s\" %s %s", disp.fname, "ASCII", "REPLACE");

    // cursor
    wmove(disp.hexwin, disp.line, disp.col);
    top_panel(disp.hexpan);

    // refresh display
    update_panels();
    doupdate();
}

void mvline(int n)
{
    disp.line += n;
}

void mvcol(int n)
{
    disp.col += n;
}

