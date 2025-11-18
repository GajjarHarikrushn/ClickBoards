#include "displayDriver.h"

uint16_t screen[DISPLAY_SIZE][DISPLAY_SIZE];

Display *initDisplay() {
    Display *d = (Display*)malloc(sizeof(Display));
    d->player = initPlayer();

    for(int i = 0; i < DISPLAY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {
            screen[i][j] = BLACK;
        }
    }

    int objectI = 0;
    int objectJ = 0;
    for(int i = d->player->r; i < PLAYER_SIZE; i++) {
        for(int j = d->player->c; j < PLAYER_SIZE; j++) {
            screen[i][j] = d->player->player[objectI][objectJ];
            objectI++;
            objectJ++;
        }
    }
    return d;
}

void drawScreen(Display *display) {
    for(int i = 0; i < DISPLAY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {
            displayDrawPixel(i,j,screen[i][j]);
        }
    }
}