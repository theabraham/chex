#include "common.h"

void ncurses_init()
{
    initscr();
    noecho();
    raw();
    keypad(stdscr, true);

    if (has_colors()) {
        start_color();
        init_pair(WHITE, COLOR_WHITE, COLOR_WHITE);
        init_pair(BLACK, COLOR_BLACK, COLOR_WHITE);
        init_pair(RED, COLOR_RED, COLOR_RED);
        init_pair(GREEN, COLOR_GREEN, COLOR_GREEN);
        init_pair(BLUE, COLOR_BLUE, COLOR_BLUE);
        init_pair(CYAN, COLOR_CYAN, COLOR_CYAN);
        init_pair(YELLOW, COLOR_YELLOW, COLOR_YELLOW);
        init_pair(MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
        init_pair(ORANGE, COLOR_YELLOW, COLOR_YELLOW);
        use_default_colors();
    } else {
        fprintf(stderr, "[Error] Your terminal does not support colors.\n");
        endwin();
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
        do { buf_draw(); } while (route(getch()));
    }

    buf_free();
    endwin();

    if (status > 0) {
        // critical error somewhere in the program
        perror(error_line);
        exit(errno);
    }

    return 0;
}

