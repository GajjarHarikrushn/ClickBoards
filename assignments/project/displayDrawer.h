#pragma once

#include "display.h"
#include "stdlib.h"
#include <math.h>

//variables used in multiple parts of the code so they are given here
//also ENEMY_COUNT is used in game.c so its is needed here
#define ENEMY_COUNT             12
#define PROJECTILE_NUM          12
#define ENEMY_PROJECTILE_NUM    6

bool game_over();
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