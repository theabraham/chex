#include "common.h"

/* Default values for BPADDR, BPLINE, and BPGRP. */
#define BYTES_PER_ADDR 8
#define BYTES_PER_LINE 16
#define BYTES_PER_GROUP 2

void ncurses_init()
{
    initscr();
    noecho();
    raw();
    keypad(stdscr, true);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(WHITE, COLOR_WHITE, -1);
        init_pair(BLACK, -1, COLOR_BLACK);
        init_pair(RED, COLOR_RED, -1);
        init_pair(GREEN, COLOR_GREEN, -1);
        init_pair(BLUE, COLOR_BLUE, -1);
        init_pair(CYAN, COLOR_CYAN, -1);
        init_pair(YELLOW, COLOR_YELLOW, -1);
        init_pair(MAGENTA, COLOR_MAGENTA, -1);
        init_pair(ORANGE, COLOR_YELLOW, -1);
    } else {
        endwin();
        fprintf(stderr, "[Error] Your terminal does not support colors.\n");
        exit(1);
    }
}

void usage()
{
    printf("usage: chex [file]\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2 || (strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
        usage(); 
        return 0;
    }

    char *filename = argv[1];
    int status = setjmp(error_pos);
    if (status == 0) {
        ncurses_init();
        buf_init(filename);
        view_init(BYTES_PER_ADDR, BYTES_PER_LINE, BYTES_PER_GROUP);
        do { view_display(); } while (route(getch()));
    }

    buf_free();
    view_free();
    endwin();

    if (status > 0) { /* Critical error somewhere in the program. */
        perror(error_line);
        exit(errno);
    }

    return 0;
}

