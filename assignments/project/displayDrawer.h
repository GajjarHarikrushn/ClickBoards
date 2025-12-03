#pragma once

#include "display.h"
#include "stdlib.h"
#include <math.h>

#define ENEMY_COUNT 12

void resetGame();

void generateSpaceBackground();
void moveBackground();
void updateDisplay();

void updateSpacePos(uint8_t joyX, uint8_t joyY);
void addSpaceship();

void addEnemies(int count) ;
void spawnEnemies();

void addProjectile();
void updateProjectile();

void addEnemyProjectile();
void updateEnemyProjectile();