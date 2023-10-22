#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define Y_MAX 25
#define X_MAX 80
#define CELL '0'
#define DEAD '-'

void render_frame_concole(int map[Y_MAX][X_MAX]);
void matrix_input(int matrix[Y_MAX][X_MAX]);
void generation(int matrix[Y_MAX][X_MAX], int matrix_new[Y_MAX][X_MAX]);
int counter_alive(int matrix[Y_MAX][X_MAX], int i, int j);
void switch_matrix(int matrix[Y_MAX][X_MAX], int matrix_new[Y_MAX][X_MAX]);
void speed(int *koef, char temp, int *end);
void ncurses_set();
void prerender(int matrix[Y_MAX][X_MAX]);
void print_end();
void begin_render();
void check_end(int matrix[Y_MAX][X_MAX], int matrix_new[Y_MAX][X_MAX], int *is_end);

int main() {
    int matrix[Y_MAX][X_MAX];
    int koef = 1;
    int is_end = 0;
    ncurses_set();
    begin_render();
    matrix_input(matrix);
    if (freopen("/dev/tty", "r", stdin)) {
        if (getchar()) {
            prerender(matrix);
            int matrix_new[Y_MAX][X_MAX];
            while (!is_end) {
                generation(matrix, matrix_new);
                render_frame_concole(matrix_new);
                refresh();
                char temp = getch();
                speed(&koef, temp, &is_end);
                usleep(100000 * koef);
                check_end(matrix, matrix_new, &is_end);
                switch_matrix(matrix, matrix_new);
            }
        }
    }
    print_end();
    if (getchar()) {
        endwin();
    }
    return 0;
}

void render_frame_concole(int map[Y_MAX][X_MAX]) {
    for (int i = 0; i < Y_MAX; i++) {
        for (int j = 0; j < X_MAX; j++) {
            if (j == X_MAX - 1) {
                if (map[i][j] == 1) {
                    move(i, j);
                    addch('O' | A_BOLD | COLOR_PAIR(1));
                    printw("\n");
                } else {
                    move(i, j);
                    addch(' ');
                    printw("\n");
                }
            } else {
                if (map[i][j] == 1) {
                    move(i, j);
                    addch('O' | A_BOLD | COLOR_PAIR(1));
                } else {
                    move(i, j);
                    addch(' ');
                }
            }
        }
    }
    mvwprintw(stdscr, 25, (X_MAX - strlen("speed up - u / speed down - d")) / 2,
              "speed up - u / speed down - d");
}

void matrix_input(int matrix[Y_MAX][X_MAX]) {
    char ch;
    for (int i = 0; i < Y_MAX; i++) {
        for (int j = 0; j < X_MAX + 1; j++) {
            if ((ch = getchar()) != EOF) {
                if (ch == '\n') {
                    break;
                }
                if (ch == DEAD) {
                    matrix[i][j] = 0;
                } else if (ch == CELL) {
                    matrix[i][j] = 1;
                }
            } else {
                break;
            }
        }
    }
}

void generation(int matrix[Y_MAX][X_MAX], int matrix_new[Y_MAX][X_MAX]) {
    for (int i = 0; i < Y_MAX; i++) {
        for (int j = 0; j < X_MAX; j++) {
            int countAlive = counter_alive(matrix, i, j);

            if (matrix[i][j] == 0) {
                if (countAlive == 3) {
                    matrix_new[i][j] = 1;
                } else {
                    matrix_new[i][j] = 0;
                }
            } else {
                if (countAlive == 2 || countAlive == 3) {
                    matrix_new[i][j] = 1;
                } else {
                    matrix_new[i][j] = 0;
                }
            }
        }
    }
}

int counter_alive(int matrix[Y_MAX][X_MAX], int i, int j) {
    int count = 0;
    int resY;
    int resX;
    for (int n = -1; n <= 1; n++) {
        for (int m = -1; m <= 1; m++) {
            resY = (n + i + Y_MAX) % Y_MAX;
            resX = (m + j + X_MAX) % X_MAX;

            if (matrix[resY][resX] == 1) {
                if (n || m) {
                    count++;
                }
            }
        }
    }
    return count;
}

void switch_matrix(int matrix[Y_MAX][X_MAX], int matrix_new[Y_MAX][X_MAX]) {
    for (int i = 0; i < Y_MAX; i++) {
        for (int j = 0; j < X_MAX; j++) {
            matrix[i][j] = matrix_new[i][j];
        }
    }
}

void speed(int *koef, char temp, int *end) {
    switch (temp) {
        case 'u':
            if (*koef != 1) *koef /= 2;
            break;
        case 'd':
            if (*koef != 16) {
                *koef *= 2;
            }
            break;
        case 'q':
            *end = 1;
    }
}

void ncurses_set() {
    initscr();
    if (!has_colors()) {
        endwin();
        printf("Цвета не поддерживаются");
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    curs_set(0);
    nodelay(stdscr, TRUE);
    noecho();
}

void prerender(int matrix[Y_MAX][X_MAX]) {
    render_frame_concole(matrix);
    refresh();
    clear();
}

void print_end() {
    clear();
    const char *mesg = "\"Game of live\"";
    const char *mesg1 = "Developed by duanebri / dewgongr / parmenid";
    mvwprintw(stdscr, Y_MAX / 2, (X_MAX - strlen(mesg)) / 2, "%s", mesg);
    mvwprintw(stdscr, Y_MAX / 2 + 1, (X_MAX - strlen(mesg1)) / 2, "%s", mesg1);
    mesg = "Please, enter any key to start...";
    mvwprintw(stdscr, Y_MAX / 2 + 3, (X_MAX - strlen(mesg)) / 2, "%s", mesg);
    refresh();
}

void begin_render() {
    const char *mesg = "Game of live";
    mvwprintw(stdscr, Y_MAX / 2, (X_MAX - strlen(mesg)) / 2, "%s", mesg);
    mesg = "Please, enter any key to start...";
    mvwprintw(stdscr, Y_MAX / 2 + 1, (X_MAX - strlen(mesg)) / 2, "%s", mesg);
    refresh();
    clear();
}

void check_end(int matrix[Y_MAX][X_MAX], int matrix_new[Y_MAX][X_MAX], int *end) {
    int flag = 0;
    for (int i = 0; i < Y_MAX; i++) {
        for (int j = 0; j < X_MAX; j++) {
            if (matrix[i][j] != matrix_new[i][j]) {
                flag++;
            }
        }
    }
    if (!flag) {
        *end = 1;
    }
}