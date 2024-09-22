#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct head {
    int posX;
    int posY;
};

struct snake {
    struct head body[1024];
    int velX;
    int velY;
    int size;
    char* texture;
};

struct food {
    char* face;
    int posX;
    int posY;
};

void updatePos(struct snake *s, WINDOW *win);
bool checkLoss(struct snake *s);
void spawnFood(struct food *fd, int maxX, int maxY);

int main() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    noecho();
    curs_set(0);

    // Create a centered box
    int height = 12, width = 40;
    int startY = (LINES - height) / 2;
    int startX = (COLS - width) / 2;
    WINDOW* win = newwin(height, width, startY, startX);
    box(win, 0, 0);
    wrefresh(win);

    struct snake p;
    p.velX = 1;
    p.velY = 0;
    p.body[0].posX = 1;
    p.body[0].posY = 1;
    p.texture = "*";
    p.size = 0;

    struct food fd;
    spawnFood(&fd, width - 2, height - 2);

    bool lost = false;

    while (!lost) {
        int key = getch();
        if (key == KEY_RIGHT && p.velX != -1) {
            p.velX = 1;
            p.velY = 0;
        }
        if (key == KEY_LEFT && p.velX != 1) {
            p.velX = -1;
            p.velY = 0;
        }
        if (key == KEY_UP && p.velY != 1) {
            p.velX = 0;
            p.velY = -1;
        }
        if (key == KEY_DOWN && p.velY != -1) {
            p.velX = 0;
            p.velY = 1;
        }

        werase(win);
        box(win, 0, 0);
        updatePos(&p, win);
        mvwaddstr(win, fd.posY, fd.posX, fd.face);
        
        if (fd.posY == p.body[0].posY && fd.posX == p.body[0].posX) {
            spawnFood(&fd, width - 2, height - 2);
            p.size += 1;
        }

        wrefresh(win);
        usleep(100000);
        lost = checkLoss(&p);
    }

    delwin(win);
    endwin();
    printf("You fucking lost\n");
    return 0;
}

void updatePos(struct snake *s, WINDOW *win) {
    for (int i = s->size; i >= 0; i--) {
        if (i == 0) {
            s->body[i].posX += s->velX;
            s->body[i].posY += s->velY;
        } else {
            s->body[i] = s->body[i - 1];
        }
        mvwaddstr(win, s->body[i].posY, s->body[i].posX, s->texture);
    }
}

bool checkLoss(struct snake *s) {
    for (int i = s->size; i >= 0; i--) {
        if (i != 0) {
            if (s->body[i].posX == s->body[0].posX &&
                s->body[i].posY == s->body[0].posY) {
                return true;
            }
        }
    }
    if (s->body[0].posX <= 0 || s->body[0].posX >= COLS - 1 ||
        s->body[0].posY <= 0 || s->body[0].posY >= LINES - 1) {
        return true;
    }
    return false;
}

void spawnFood(struct food *fd, int maxX, int maxY) {
    fd->posX = rand() % maxX + 1;
    fd->posY = rand() % maxY + 1;
    fd->face = "&";
}

