#include "common.h"
#include "buffer.h"

static struct {
    char *filename; // file name
    FILE *fp;       // file-pointer
    char *mem;      // memory block to store the file's contents
    size_t size;    // memory block's byte size
    size_t index;   // current position in memory block
    modes_t mode;   // current information mode
    states_t state; // current editing state
} buf;

// Get the give file's byte size.
size_t filesize(FILE *fp)
{
    size_t size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    return size;
}

void buf_init(char *filename)
{
    buf.filename = filename;
    buf.fp = fopen(filename, "r+");
    buf.size = filesize(buf.fp);
    buf.index = 0;
    buf.mem = malloc(buf.size*sizeof(char));
    buf.mode = HEX;
    buf.state = ESCAPE;
    view_init();

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
    view_draw(buf.filename, buf.mem, buf.size, buf.index, buf.mode, buf.state);
}

//void buf_setpos(int pos)
//    buf.index = 0;
//{
//    if (pos >= 0 && pos < buf.size) {
//        buf.pos = pos;
//    }
//}
//
//void buf_nudge(int dist)
//{
//    buf_setpos(buf.pos+dist);
//}
//
//int buf_getpos()
//{
//    return buf.pos;
//}
//
//void buf_setch(char ch, enum half side)
//{
//    char new = buf.mem[buf.pos];
//
//    if (side == LOW) {
//        new = 0xF0 & new;
//        new = new | ch;
//    } else if (side == HIGH) {
//        new &= 0x0F;
//        ch <<4;
//        new |= ch;
//    } else {
//        new = ch;
//    }
//
//    buf.mem[buf.pos] = new;
//}
//
//int buf_getch()
//{
//    return buf.mem[buf.pos];
//}
