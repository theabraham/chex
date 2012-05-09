#include "common.h"
#include "buffer.h"

#define BYTES_PER_ADDR    8   // default `bpaddr`
#define BYTES_PER_LINE    16  // default `bpline`
#define BYTES_PER_SEGMENT 1   // default `bpseg`

static struct {
    char *filename; 
    FILE *fp;           // file-pointer
    unsigned char *mem; // memory block to store the file's contents
    size_t size;        // memory block's byte size
    size_t index;       // current position in memory block
    bool nybble;        // targeting the higher-or-lower order bits at the current index (for HEX mode)
    int bpaddr;         // bytes of data to show for each address
    int bpline;         // bytes of data to show per line (ascii or hex)
    int bpseg;          // bytes of data to show per segment (hex only)
    modes_t mode;       // current editing mode
    states_t state;     // current state.
} buf;

// Get the given file's byte size.
static long filesize(FILE *fp)
{
    long size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    return size;
}

// Return the actual value a hex character represents.
static char hex2byte(char ch) {
    char hexstr[2] = { ch, '\0' };
    return strtol(hexstr, NULL, 16);
}

// Get what line the view's cursor should be on relative to the buffer's current
// index and bytes-per-line being shown.
static int getline()
{
    return buf.index/buf.bpline;
}

// Get what column the view's cursor should be on relative to the buffer's current
// index, bytes-per-line being shown, and mode.
static int getcol()
{
    int col = buf.index%buf.bpline;
    if (buf.mode == HEX)
        col = col*3+(buf.nybble?1:0);
    return col;
}

// Checks whether the given `index` is within the buffer's bounds.
static bool inbounds(int index)
{
    return (index >= 0 && index < buf.size);
}

static void mvline(int n)
{
    if (!(n == -1 || n == 1)) // ghetto, expects +/- 1
        return;

    int index = buf.index+(n*buf.bpline);
    if (inbounds(index))
        buf.index = index;
}

static void mvcol(int n)
{
    if (!(n == -1 || n == 1)) // ghetto, expects +/- 1
        return;

    int index = buf.index;
    bool nybble = buf.nybble;
    if (buf.mode == HEX) {
        if (nybble) {
            nybble = false;
            if (n == 1)
                index++;
        } else {
            nybble = true;
            if (n == -1)
                index--;
        }
    } else {
        index += n;
    }

    if (inbounds(index)) {
        buf.index = index;
        buf.nybble = nybble;
    }
}


void buf_init(char *filename)
{
    buf.filename = filename;
    buf.fp = fopen(filename, "r+");
    buf.size = filesize(buf.fp);
    buf.index = 0;
    buf.mem = malloc(buf.size*sizeof(char));
    buf.bpaddr = BYTES_PER_ADDR;
    buf.bpline = BYTES_PER_LINE;
    buf.bpseg = BYTES_PER_SEGMENT;
    buf.mode = HEX;
    buf.state = ESCAPE;
    view_init(buf.bpaddr, buf.bpline, buf.bpseg);

    // read file into buffer
    int ch, i = 0;
    while ((ch = fgetc(buf.fp)) != EOF) {
        buf.mem[i++] = ch;
    }
}

void buf_free()
{
    fclose(buf.fp);
    free(buf.mem);
    view_free();
}

void buf_draw()
{
    view_display(buf.mem, buf.size); 
    view_msg(buf.filename, buf.mode, buf.state, buf.index, buf.size);
    view_cursor(getline(), getcol(), buf.mode);
    view_update();
}

void buf_move(int line, int col)
{
    mvline(line);
    mvcol(col);
}

modes_t buf_getmode()
{
    return buf.mode;
}

void buf_setmode(modes_t mode)
{
    if (mode == ASCII)
        buf.nybble = false;
    buf.mode = mode;
}

states_t buf_getstate()
{
    return buf.state;
}

void buf_setstate(states_t state)
{
    buf.state = state;
}

void buf_repc(char ch)
{
    if (buf.mode == HEX && ishexnumber(ch)) {
        char orig = buf.mem[buf.index];
        ch = hex2byte(ch);
        if (buf.nybble) {
            // write to low bits
            orig &= 0xF0;
            ch |= orig;
            buf.mem[buf.index++] = ch;
            buf.nybble = false;
        } else {
            // write to high bits
            orig &= 0x0F;
            ch <<= 4;
            ch |= orig;
            buf.mem[buf.index] = ch;
            buf.nybble = true;
        }
    } else if (buf.mode == ASCII) {
        // write to whole byte
        buf.mem[buf.index++] = ch;
    }
}

