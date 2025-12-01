#pragma once

#include "display.h"
#include "stdlib.h"
#include <math.h>

void generateSpaceBackground();
void updateDisplay();

void updateSpacePos(uint8_t joyX, uint8_t joyY);
void addSpaceship();

void addEnemies() ;
void spawnEnemies();

void addProjectile();
void updateProjectile();