#pragma once

#include "stdbool.h"
#include <stdlib.h>
#include <stdint.h>

#define PLAYER_SIZE 3

typedef struct {
    int r;
    int c;
    bool player[PLAYER_SIZE][PLAYER_SIZE];
} Player;

Player *initPlayer();