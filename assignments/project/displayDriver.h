#pragma once

#include "display.h"
#include "stdbool.h"
#include "player.h"
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    Player *player;
} Display;

Display *initDisplay();
void drawScreen();