#ifdef _WIN32
    #include <pdcurses.h>
    #include <windows.h>
#else
    #include <curses.h>
    #include <unistd.h>  
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct head {
    int posX;
    int posY;
};

struct snake {
    struct head body[1024];
    int velX;
    int velY;
    int size;
    char* texture_body;
    char* texture_head;
};

struct food {
    char* texture;
    int posX;
    int posY;
};

void updatePos(struct snake *s, WINDOW *win);
bool checkLoss(struct snake *s, int startX, int startY, int width, int height);
void spawnFood(struct food *fd, int maxX, int maxY);

void sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);  // Windows Sleep takes milliseconds
#else
    usleep(milliseconds * 1000);  // Linux usleep takes microseconds
#endif
}

int main() {
    initscr();
    cbreak();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    noecho();
    curs_set(0);

    // Start screen
    int height = 24;
    int width = 80;
    int startY = (LINES - height) / 2; // Half of the screen - box
    int startX = (COLS - width) / 2; // Half of the screen - box
    WINDOW* win = newwin(height, width, startY, startX);
    box(win, 0, 0);
    wrefresh(win);

    struct snake p;
    p.velX = 1;
    p.velY = 0;
    p.body[0].posX = 1;
    p.body[0].posY = 1;
    p.texture_body = "*";
    p.texture_head = ">";
    p.size = 5;

    struct food fd;
    spawnFood(&fd, width - 2, height - 2);

    bool lost = false;
    int score = 0;
    while (!lost) {
        mvprintw(startY + height, startX, "Score: %d", score);
        int key = getch();
        if (key == KEY_RIGHT && p.velX == 0) {
            p.velX = 1;
            p.velY = 0;
        }
        if (key == KEY_LEFT && p.velX == 0) {
            p.velX = -1;
            p.velY = 0;
        }
        if (key == KEY_UP && p.velY == 0) {
            p.velX = 0;
            p.velY = -1;
        }
        if (key == KEY_DOWN && p.velY == 0) {
            p.velX = 0;
            p.velY = 1;
        }

        werase(win);
        box(win, 0, 0);
        updatePos(&p, win);
        mvwaddstr(win, fd.posY, fd.posX, fd.texture);

        if (fd.posY == p.body[0].posY && fd.posX == p.body[0].posX) {
            spawnFood(&fd, width - 2, height - 2);
            p.size += 1;
            score++;
        }

        wrefresh(win);

        // Cross-platform sleep
        sleep_ms(100);  // 100 milliseconds delay

        lost = checkLoss(&p, startX, startY, width, height);
    }

    delwin(win);
    endwin();
    printf("You lost!\n");
    return 0;
}

void updatePos(struct snake *s, WINDOW *win) {
    for (int i = s->size; i >= 0; i--) {
        if (i == 0) {
            s->body[i].posX += s->velX;
            s->body[i].posY += s->velY;
            if (s->velX == 1) {
                s->texture_head = ">";
            } else if (s->velX == -1) {
                s->texture_head = "<";
            } else if (s->velY == 1) {
                s->texture_head = "v";
            } else if (s->velY == -1) {
                s->texture_head = "^";
            }
            mvwaddstr(win, s->body[i].posY, s->body[i].posX, s->texture_head);
        } else {
            s->body[i] = s->body[i - 1];
            mvwaddstr(win, s->body[i].posY, s->body[i].posX, s->texture_body);
        }
    }
}

bool checkLoss(struct snake *s, int startX, int startY, int width, int height) {
    for (int i = s->size; i >= 0; i--) {
        if (i != 0) {
            if (s->body[i].posX == s->body[0].posX && s->body[i].posY == s->body[0].posY) {
                return true;
            }
        }
    }
    if (s->body[0].posX <= 0 || s->body[0].posX >= width - 1 ||
        s->body[0].posY <= 0 || s->body[0].posY >= height - 1) {
        return true;
    }
    return false;
}

void spawnFood(struct food *fd, int maxX, int maxY) {
    fd->posX = rand() % maxX + 1;
    fd->posY = rand() % maxY + 1;
    fd->texture = "&";
}

