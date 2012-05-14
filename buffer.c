#include "common.h"
#include "buffer.h"

#define BYTES_PER_ADDR    8  /* Default bytes shown per address (BPADDR) */
#define BYTES_PER_LINE    16 /* Default bytes shown per line (BPLINE) */
#define BYTES_PER_SEGMENT 2  /* Default bytes shown per hex segment (BPSEG) */

#define TMP_LINES (LINES)

struct {
    char *filename; 
    FILE *fp;           /* File-pointer returned after opening FILENAME. */ 
    unsigned char *mem; /* Memory block to store FP's contents. */
    size_t size;        /* Byte size of MEM. */
    size_t index;       /* Current position in MEM. */
    bool nybble;        /* Editing the higher or lower order bits at INDEX. */
    int edge;           /* Line where the top edge of the visible buffer is. */
    int bpaddr;         /* Bytes to show per address line. */
    int bpline;         /* Bytes to show per line. */
    int bpgrp;          /* Bytes to show per space delimited hex group. */
    modes_t mode;       /* Current mode equal to ASCII or HEX. */
    states_t state;     /* Current state equal to ESCAPE or REPLACE. */
} buf;

/* Get FP's size in bytes; keeps FP's file position indicator at the beginning.
   Return -1 on error. */

static long filesize(FILE *fp)
{
    long size;
    if (fseek(fp, 0, SEEK_END) == -1)
        return -1; 
    if ((size = ftell(fp)) == -1)
        return -1;
    rewind(fp);
    return size;
}

/* If CH is a hex character, return its bit value; otherwise, return -1. */

static char hextobit(char ch) {
    char hexstr[2] = { ch, '\0' };
    if (!ishexnumber(ch))
        return -1;
    return strtol(hexstr, NULL, 16);
}

// Move the edge up or down by `n` lines while making sure it's inbounds.
static void mvedge(int n)
{
    int edge = buf.edge + n;
    if (edge >= 0 && edge < (buf.size / buf.bpline))
        buf.edge = edge;
}

// Return the index representation of the edge's top.
static size_t edgetop()
{
    return buf.edge * buf.bpline;
}

// Return the index representation of the edge's bottom.
static size_t edgebot()
{
    size_t bottom = ((buf.edge + TMP_LINES) * buf.bpline) + (buf.bpline - 1);
    if (bottom >= buf.size)
        return buf.size - 1;
    return bottom;
}

// Move the index forward or backward by `n` positions. Make sure the index is
// in bounds and within view of the edge's top and bottom bounds.
static bool setindex(int index, bool nybble)
{
    if (index < 0 || index >= buf.size) {
        return false;
    } else {
        while (index < edgetop()) mvedge(-1);
        while (index > edgebot()) mvedge(+1);
        buf.index = index;
        buf.nybble = nybble;
        return true;
    }
}

static void setchar(char ch)
{
    if (buf.mode == HEX && ishexnumber(ch)) {
        char orig = buf.mem[buf.index];
        ch = hex2byte(ch);
        if (buf.nybble) {
            // write to low bits
            orig &= 0xF0;
            ch |= orig;
            buf.mem[buf.index] = ch;
        } else {
            // write to high bits
            orig &= 0x0F;
            ch <<= 4;
            ch |= orig;
            buf.mem[buf.index] = ch;
        }
    } else if (buf.mode == ASCII) {
        // write to whole byte
        buf.mem[buf.index] = ch;
    }
}

static void mvline(int n)
{
    setindex(buf.index + (n * buf.bpline), buf.nybble);
}

static void mvcol(int n)
{
    if (buf.mode == HEX) {
        // in hex mode, 2-columns represent one index value, and odd columns
        // represent the lower nybble of the current byte
        int index = buf.index + (n / 2);
        if (n % 2 != 0) {
            if (buf.nybble) {
                setindex(index + (n > 0 ? 1 : 0), false);
            } else {
                setindex(index - (n > 0 ? 0 : 1), true);
            }
        } else {
            setindex(index, false);
        }
    } else {
        // ascii mode is simply one-to-one
        setindex(buf.index + n, false);
    }
}

// Line which the cursor should be on.
static int getline()
{
    return (buf.index / buf.bpline) - buf.edge;
}

// Column which the cursor should be on.
static int getcol()
{
    int col = buf.index % buf.bpline;
    if (buf.mode == HEX)
        col = (col * 2) + (col / buf.bpseg) + (buf.nybble ? 1 : 0);
    return col;
}

void buf_init(char *filename)
{
    // open and read file
    buf.filename = filename;
    buf.fp = fopen(filename, "r+");
    CHECK(buf.fp != NULL);
    buf.size = filesize(buf.fp);
    CHECK(buf.size != -1);
    buf.mem = malloc(buf.size*sizeof(char));
    CHECK(buf.mem != NULL);
    fread(buf.mem, sizeof(char), buf.size, buf.fp);
    CHECK(ferror(buf.fp) == 0);
    rewind(buf.fp);

    // set default initial values
    buf.index = 0;
    buf.nybble = false;
    buf.edge = 0;
    buf.bpaddr = BYTES_PER_ADDR;
    buf.bpline = BYTES_PER_LINE;
    buf.bpseg = BYTES_PER_SEGMENT;
    buf.mode = HEX;
    buf.state = ESCAPE;
    view_init(buf.bpaddr, buf.bpline, buf.bpseg);
}

void buf_free()
{
    fclose(buf.fp);
    free(buf.mem);
    view_free();
}

void buf_write()
{
    size_t write_size;
    rewind(buf.fp);
    write_size = fwrite(buf.mem, sizeof(char), buf.size, buf.fp);
    if (write_size < buf.size) /* Write error. */
        clearerr(buf.fp);
}

void buf_revert()
{
    rewind(buf.fp);
    fread(buf.mem, sizeof(char), buf.size, buf.fp);
    if (ferror(buf.fp) != 0) /* Read error. */
        clearerr(buf.fp);
}

void buf_draw()
{
    view_clear();
    view_display(buf.mem, buf.size, edgetop(), edgebot(), buf.bpaddr, buf.bpline, buf.bpseg); 
    view_msg(buf.filename, buf.mode, buf.state, buf.index, buf.size);
    view_cursor(getline(), getcol(), buf.mode);
    view_update();
}

