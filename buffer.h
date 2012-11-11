#ifndef BUFFER_T
#define BUFFER_T

struct {
    char *filename; 
    FILE *fp;           /* File-pointer returned after opening FILENAME. */ 
    unsigned char *mem; /* Memory block to store FP's contents. */
    size_t size;        /* Byte size of MEM. */
    size_t index;       /* Current position in MEM. */
    bool nybble;        /* Editing the higher or lower order bits at INDEX. */
    modes_t mode;       /* Current mode equal to either ASCII or HEX. */
    states_t state;     /* Current state equal to either ESCAPE or REPLACE. */
} buf;

/* Initialize the buffer with information from FILENAME. */
void buf_init(char *filename);

/* Close the open file and free the buffer's memory. */
void buf_free();

/* Overwrite the original file on disk with the buffer's current memory. */
void buf_write();

/* Revert the buffer's current memory to reflect the original file's contents.  */
void buf_revert();

/* Set the buffer's INDEX and NYBBLE. If the given INDEX is out of bounds, set it
   to either the beginning or the end of the buffer. */
void buf_setindex(int index, bool nybble);

/* Set the value at the buffer's current INDEX and NYBBLE to CH which can either
   be a HEX or ASCII character. */
void buf_putchar(char ch);

/* Restore the HEX or ASCII character at the current INDEX and NYBBLE to the original
   file's value. */
void buf_revertchar();

#endif
