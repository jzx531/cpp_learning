#include <curses.h>

int main() {
    initscr();
    printw("Hello, PDCurses with MinGW!");
    refresh();
    getch();
    endwin();
    return 0;
}

