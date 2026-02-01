#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <curses.h>        // ← 改为 curses.h（PDCurses）
#include <string.h>

// 移除 SDL 相关（简化版，聚焦 PDCurses）
// #if WITH_SDL ... #endif 全部删除

#include "tetris.h"
#include "util.h"

/*
  2 columns per cell makes the game much nicer.
 */
#define COLS_PER_CELL 2

/*
  Macro to print a cell of a specific type to a window.
  注意：PDCurses 的 A_REVERSE 在 Windows 控制台效果可能不同，
  若颜色不理想，可改用 COLOR_PAIR + 普通字符。
 */
#define ADD_BLOCK(w, x) \
    waddch((w), ' ' | COLOR_PAIR(x)); \
    waddch((w), ' ' | COLOR_PAIR(x))

#define ADD_EMPTY(w) \
    waddch((w), ' '); \
    waddch((w), ' ')

/*
  Print the tetris board onto the PDCurses window.
 */
void display_board(WINDOW *w, tetris_game *obj)
{
    int i, j;
    box(w, 0, 0);
    for (i = 0; i < obj->rows; i++) {
        wmove(w, 1 + i, 1);
        for (j = 0; j < obj->cols; j++) {
            if (TC_IS_FILLED(tg_get(obj, i, j))) {
                ADD_BLOCK(w, tg_get(obj, i, j));
            } else {
                ADD_EMPTY(w);
            }
        }
    }
    wnoutrefresh(w);
}

/*
  Display a tetris piece in a dedicated window.
*/
void display_piece(WINDOW *w, tetris_block block)
{
    int b;
    tetris_location c;
    wclear(w);
    box(w, 0, 0);
    if (block.typ == -1) {
        wnoutrefresh(w);
        return;
    }
    for (b = 0; b < TETRIS; b++) {
        c = TETROMINOS[block.typ][block.ori][b];
        // 注意：c.row/c.col 是偏移，确保窗口足够大
        int row = c.row + 1;
        int col = c.col * COLS_PER_CELL + 1;
        if (row >= 0 && col >= 0) {
            wmove(w, row, col);
            ADD_BLOCK(w, TYPE_TO_CELL(block.typ));
        }
    }
    wnoutrefresh(w);
}

/*
  Display score information in a dedicated window.
 */
void display_score(WINDOW *w, tetris_game *tg)
{
    wclear(w);
    box(w, 0, 0);
    wprintw(w, "Score\n%d\n", tg->points);
    wprintw(w, "Level\n%d\n", tg->level);
    wprintw(w, "Lines\n%d\n", LINES_PER_LEVEL - tg->lines_remaining); // 更合理显示
    wnoutrefresh(w);
}

/*
  Boss mode! Make it look like you're doing work.
 */
void boss_mode(void)
{
    clear();
    printw("user@workstation-312:～/Documents/presentation $ ls -l\n"
           "total 528\n"
           "drwxr-xr-x 2 user users   4096 Jun  9 17:05 .\n"
           "drwxr-xr-x 4 user users   4096 Jun 10 09:52 ..\n"
           "-rw-r--r-- 1 user users  88583 Jun  9 14:13 figure1.png\n"
           "-rw-r--r-- 1 user users  65357 Jun  9 15:40 figure2.png\n"
           "-rw-r--r-- 1 user users   4469 Jun  9 16:17 presentation.aux\n"
           "-rw-r--r-- 1 user users  42858 Jun  9 16:17 presentation.log\n"
           "-rw-r--r-- 1 user users   2516 Jun  9 16:17 presentation.nav\n"
           "-rw-r--r-- 1 user users    183 Jun  9 16:17 presentation.out\n"
           "-rw-r--r-- 1 user users 349607 Jun  9 16:17 presentation.pdf\n"
           "-rw-r--r-- 1 user users      0 Jun  9 16:17 presentation.snm\n"
           "-rw-r--r-- 1 user users   9284 Jun  9 17:05 presentation.tex\n"
           "-rw-r--r-- 1 user users    229 Jun  9 16:17 presentation.toc\n"
           "\n"
           "user@workstation-312:～/Documents/presentation $ ");
    echo();
    timeout(-1);
    while (getch() != KEY_F(1)); // 按 F1 退出
    timeout(0);
    noecho();
    clear();
}

/*
  Save and exit the game.
 */
void save(tetris_game *game, WINDOW *w)
{
    FILE *f;

    wclear(w);
    box(w, 0, 0);
    wmove(w, 1, 1);
    wprintw(w, "Save and exit? [Y/n] ");
    wrefresh(w);
    timeout(-1);
    int ch = getch();
    timeout(0);
    if (ch == 'n' || ch == 'N') {
        return;
    }
    f = fopen("tetris.save", "w");
    if (f) {
        tg_save(game, f);
        fclose(f);
        printw("\nGame saved to \"tetris.save\".\n");
        printw("Resume by passing the filename as an argument.\n");
    }
    endwin();
    exit(EXIT_SUCCESS);
}

/*
  Do the PDCurses initialization steps for color blocks.
 */
void init_colors(void)
{
    if (!has_colors()) {
        printw("Terminal does not support color!\n");
        return;
    }
    start_color();

    // PDCurses on Windows supports 16 colors
    init_pair(TC_CELLI, COLOR_CYAN, COLOR_BLACK);
    init_pair(TC_CELLJ, COLOR_BLUE, COLOR_BLACK);
    init_pair(TC_CELLL, COLOR_WHITE, COLOR_BLACK);
    init_pair(TC_CELLO, COLOR_YELLOW, COLOR_BLACK);
    init_pair(TC_CELLS, COLOR_GREEN, COLOR_BLACK);
    init_pair(TC_CELLT, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(TC_CELLZ, COLOR_RED, COLOR_BLACK);
}

/*
  Main tetris game!
 */
int main(int argc, char **argv)
{
    tetris_game *tg;
    tetris_move move = TM_NONE;
    bool running = true;
    WINDOW *board, *next, *hold, *score;

    // Load file if given a filename.
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f == NULL) {
            perror("tetris");
            exit(EXIT_FAILURE);
        }
        tg = tg_load(f);
        fclose(f);
    } else {
        tg = tg_create(22, 10);
    }

    // PDCurses initialization:
    initscr();             // initialize PDCurses
    cbreak();              // disable line buffering
    noecho();              // don't echo input
    keypad(stdscr, TRUE);  // enable arrow keys, F-keys
    timeout(0);            // non-blocking getch()
    curs_set(0);           // hide cursor
    init_colors();         // setup colors

    // Create windows
    board = newwin(tg->rows + 2, 2 * tg->cols + 2, 0, 0);
    next  = newwin(6, 10, 0, 2 * (tg->cols + 1) + 1);
    hold  = newwin(6, 10, 7, 2 * (tg->cols + 1) + 1);
    score = newwin(6, 10, 14, 2 * (tg->cols + 1) + 1);

    // Game loop
    while (running) {
        running = tg_tick(tg, move);
        display_board(board, tg);
        display_piece(next, tg->next);
        display_piece(hold, tg->stored);
        display_score(score, tg);
        doupdate();

        // 使用你之前修正的 sleep_milli（基于 Sleep()）
        sleep_milli(10);

        switch (getch()) {
        case KEY_LEFT:   move = TM_LEFT; break;
        case KEY_RIGHT:  move = TM_RIGHT; break;
        case KEY_UP:     move = TM_CLOCK; break;
        case KEY_DOWN:   move = TM_DROP; break;
        case 'q': case 'Q':
            running = false;
            move = TM_NONE;
            break;
        case 'p': case 'P':
            wclear(board);
            box(board, 0, 0);
            wmove(board, tg->rows/2, (tg->cols*COLS_PER_CELL - 6)/2);
            wprintw(board, "PAUSED");
            wrefresh(board);
            timeout(-1);
            getch(); // wait for any key
            timeout(0);
            move = TM_NONE;
            break;
        case 'b': case 'B':
            boss_mode();
            move = TM_NONE;
            break;
        case 's': case 'S':
            save(tg, board);
            move = TM_NONE;
            break;
        case ' ':        move = TM_HOLD; break;
        default:         move = TM_NONE;
        }
    }

    // Cleanup
    delwin(board); delwin(next); delwin(hold); delwin(score);
    endwin();

    printf("Game over!\n");
    printf("You finished with %d points on level %d.\n", tg->points, tg->level);
    tg_delete(tg);
    return 0;
}