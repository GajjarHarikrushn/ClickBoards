#include "player.h"

#define BLACK 0x0000
#define COLOR 0x01ff

uint16_t defaultShape[PLAYER_SIZE][PLAYER_SIZE] = {{BLACK,COLOR,BLACK},
                                                    {COLOR,COLOR,COLOR},
                                                    {COLOR,COLOR,COLOR}};

Player *initPlayer() {
    Player *p = (Player*)malloc(sizeof(Player));
    for(int i = 0; i < PLAYER_SIZE; i++) {
        for(int j = 0; j < PLAYER_SIZE; j++) {
            p->player[i][j] = defaultShape[i][j];
        }
    }
    return p;
}
