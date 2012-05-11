#include "common.h"
#include "buffer.h"

#define BYTES_PER_ADDR    8   // default `bpaddr`
#define BYTES_PER_LINE    16  // default `bpline`
#define BYTES_PER_SEGMENT 2   // default `bpseg`

#define TMP_LINES 8

static struct {
    char *filename; 
    FILE *fp;           // file-pointer
    unsigned char *mem; // memory block to store the file's contents
    size_t size;        // memory block's byte size
    size_t index;       // current position in memory block
    bool nybble;        // targeting the higher-or-lower order bits at the current index (for HEX mode)
    int edge;           // what line the buffer's top edge is on (bottom edge is implicit)
    int bpaddr;         // bytes of data to show for each address
    int bpline;         // bytes of data to show per line (ascii or hex)
    int bpseg;          // bytes of data to show per segment (hex only)
    modes_t mode;       // current editing mode
    states_t state;     // current state
} buf;

// Get the given file's byte size; set fp's file position indicator to the beginning.
static long filesize(FILE *fp)
{
    long size;
    rewind(fp);
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

// Move the edge up or down by `n` lines while making sure it's inbounds.
static void mvedge(int n)
{
    int edge = buf.edge + n;
    if (edge >= 0 && edge < (buf.size/buf.bpline))
        buf.edge = edge;
}

// Return the index representation of where the edge's top start.
static size_t gettop()
{
    return buf.edge*buf.bpline;
}

// Return the index representation of where the edge's bottom ends.
static size_t getbottom()
{
    size_t bottom = ((buf.edge+TMP_LINES)*buf.bpline)+(buf.bpline-1);
    if (bottom >= buf.size)
        return buf.size-1;
    return bottom;
}

// Move the index forward or backward by `n` positions. Make sure the index is
// in bounds and within view of the edge's top and bottom bounds.
static bool setindex(int index)
{
    if (index < 0 || index >= buf.size) {
        return false;
    } else {
        while (index < gettop()) mvedge(-1);
        while (index > getbottom()) mvedge(+1);
        buf.index = index;
        return true;
    }
}

// Get what line the view's cursor should be on relative to the buffer's current
// index and bytes-per-line being shown.
static int getline()
{
    return (buf.index / buf.bpline) - buf.edge;
}

// Get what column the view's cursor should be on relative to the buffer's current
// index, bytes-per-line being shown, and mode.
static int getcol()
{
    int col = buf.index%buf.bpline;
    if (buf.mode == HEX)
        col = (col*2)+(col/buf.bpseg)+(buf.nybble?1:0);
    return col;
}

static void mvline(int n)
{
    int index = buf.index+(n*buf.bpline);
    setindex(index);
}

static void mvcol(int n)
{
    int index = buf.index;
    bool nybble = buf.nybble;
    if (buf.mode == HEX) {
        // in hex mode, 2-columns represent one index value, and odd columns
        // represent the lower nybble of the current byte
        index += n/2;
        if (n%2 != 0) {
            if (nybble) {
                nybble = false;
                if (n > 0)
                    index++;
            } else {
                nybble = true;
                if (n < 0)
                    index--;
            }
        }
    } else {
        index += n;
    }

    if (setindex(index)) {
        buf.nybble = nybble;
    }
}

void buf_init(char *filename)
{
    buf.filename = filename;
    buf.fp = fopen(filename, "r+b");
    buf.size = filesize(buf.fp);
    buf.index = 0;
    buf.nybble = false;
    buf.edge = 0;
    buf.mem = malloc(buf.size*sizeof(char));
    fread(buf.mem, sizeof(char), buf.size, buf.fp); // read file
    rewind(buf.fp);
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
    fwrite(buf.mem, sizeof(char), buf.size, buf.fp);
}

void buf_revert()
{
    return;
}

void buf_draw()
{
    view_clear();
    view_display(buf.mem, buf.size, gettop(), getbottom(), buf.bpaddr, buf.bpline, buf.bpseg); 
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
    int index = buf.index;
    bool nybble = buf.nybble;
    if (buf.mode == HEX && ishexnumber(ch)) {
        char orig = buf.mem[index];
        ch = hex2byte(ch);
        if (nybble) {
            // write to low bits
            orig &= 0xF0;
            ch |= orig;
            buf.mem[index++] = ch;
            nybble = false;
        } else {
            // write to high bits
            orig &= 0x0F;
            ch <<= 4;
            ch |= orig;
            buf.mem[index] = ch;
            nybble = true;
        }
    } else if (buf.mode == ASCII) {
        // write to whole byte
        buf.mem[index++] = ch;
    }

    if (setindex(index)) {
        buf.nybble = nybble;
    }
}

void buf_begline()
{
    setindex(buf.index-(buf.index%buf.bpline));
    buf.nybble = false;
}

void buf_endline()
{
    int index = (buf.index - (buf.index % buf.bpline)) + (buf.bpline - 1);
    if (index >= buf.size)
        index = buf.size - 1;
    if (setindex(index))
        buf.nybble = true;
}

void buf_nextseg()
{
    int index = buf.index+(buf.bpseg-(buf.index%buf.bpseg));
    if (setindex(index)) {
        buf.nybble = false;
    }
}

void buf_prevseg()
{
    int index = buf.index;
    if (index%buf.bpseg) {
        // go to beginning of current segment
        index -= index%buf.bpseg;
    } else {
        // go to beginning of next segment
        index -= buf.bpseg;
    }

    if (setindex(index)) {
        buf.nybble = false;
    }
}

void buf_beg()
{
    setindex(0);
    buf.nybble = false;
}

void buf_end()
{
    setindex(buf.size - (buf.size % buf.bpline));
    buf.nybble = false;
}

void buf_nexthalf()
{
    int index = buf.index + ((TMP_LINES / 2) * buf.bpline);
    if (!setindex(index))
        buf_end();
}

void buf_prevhalf()
{
    int index = buf.index - ((TMP_LINES / 2) * buf.bpline);
    if (!setindex(index))
        buf_beg();
}
