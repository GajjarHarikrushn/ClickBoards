#include "displayDrawer.h"
#include <stdbool.h>


//display variables
bool display_update_needed = true;
uint16_t background[DISPLAY_SIZE][DISPLAY_SIZE] = {0};
uint16_t screen[DISPLAY_SIZE][DISPLAY_SIZE];

void generateSpaceBackground()
{
    static const uint8_t sin8[256] = {
        128, 130, 133, 136, 139, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173,
        176, 178, 181, 184, 187, 190, 192, 195, 198, 200, 203, 205, 208, 210, 212, 215,
        217, 219, 221, 223, 225, 227, 229, 231, 232, 234, 236, 237, 238, 240, 241, 242,
        243, 244, 245, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250, 250, 250, 250,
        250, 250, 250, 250, 250, 249, 249, 249, 248, 248, 247, 247, 246, 245, 244, 243,
        242, 241, 240, 238, 237, 236, 234, 232, 231, 229, 227, 225, 223, 221, 219, 217,
        215, 212, 210, 208, 205, 203, 200, 198, 195, 192, 190, 187, 184, 181, 178, 176,
        173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 139, 136, 133, 130, 128,
        125, 122, 119, 116, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79,
        76, 73, 71, 68, 65, 62, 60, 57, 55, 52, 49, 47, 45, 43, 41, 39,
        37, 35, 33, 31, 29, 28, 26, 24, 23, 21, 20, 18, 17, 15, 14, 13,
        12, 11, 10, 9, 8, 8, 7, 7, 6, 6, 6, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 6, 6, 6, 7, 7, 8, 8, 9, 10, 11, 12,
        13, 14, 15, 17, 18, 20, 21, 23, 24, 26, 28, 29, 31, 33, 35, 37,
        39, 41, 43, 45, 47, 49, 52, 55, 57, 60, 62, 65, 68, 71, 73, 76,
        79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 116, 119, 122, 125};

    for (int y = 0; y < 96; y++) {
        for (int x = 0; x < 96; x++) {
            // Base dark space (deep blue-purple)
            uint8_t r = 8 + (x >> 4); // subtle red gradient
            uint8_t g = 4 + (y >> 5);
            uint8_t b = 20 + ((95 - y) >> 2); // brighter blue at top

            // Soft nebula using integer noise
            uint32_t n1 = (x * 13 + y * 37) & 255;
            uint32_t n2 = (x * 27 + y * 71) & 255;
            uint32_t n3 = (x * 51 + y * 19) & 255;
            uint32_t nebula = (sin8[n1] + sin8[n2] + sin8[n3]) >> 4; // 0..48

            // Add some purple glow
            r += nebula >> 1;
            g += nebula >> 2;
            b += nebula >> 1;

            // Stars — fixed positions using magic numbers (looks random)
            uint32_t star_hash = (x * 9631 + y * 8977);
            if ((star_hash & 0xFF) < 3) { // ~1.2% chance → bright star
                r = g = b = 255;
            }
            else if ((star_hash & 0x1FF) < 5) { // medium stars
                r += 120;
                g += 120;
                b += 140;
            }
            else if ((star_hash & 0x3FF) < 8) { // tiny twinkles
                r += 80;
                g += 80;
                b += 100;
            }

            // Clamp
            if (r > 255)
                r = 255;
            if (g > 255)
                g = 255;
            if (b > 255)
                b = 255;

            // Convert to RGB565
            background[y][x] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
            screen[y][x] = background[y][x];
        }
    }
}

void updateDisplay() {
    if (display_update_needed)
    {
        drawArray(0, DISPLAY_SIZE, 0, DISPLAY_SIZE, *screen);
        display_update_needed = false;
    }
}



//---------------------------------Everything related to spaceship---------------------------------
//spaceship variables
#define SHIP_SIZE       16
#define MAX_SHIP_POS    (DISPLAY_SIZE - SHIP_SIZE)

uint8_t spaceX = DISPLAY_SIZE - SHIP_SIZE * 1.5;
uint8_t prevSpaceX = 0;
uint8_t spaceY = DISPLAY_SIZE / 2 - SHIP_SIZE / 2;
uint8_t prevSpaceY = 0;
bool spaceship_update_needed = true;
const uint16_t spaceship[16][16] = {
    {0x0000,0x0000,0x0000,0x0000,0x0000,0x7BEF,0x7BEF,0x0000,0x0000,0x7BEF,0x7BEF,0x0000,0x0000,0x0000,0x0000,0x0000},
    {0x0000,0x0000,0x0000,0x0000,0x7BEF,0xFFFF,0xFFFF,0x7BEF,0x7BEF,0xFFFF,0xFFFF,0x7BEF,0x0000,0x0000,0x0000,0x0000},
    {0x0000,0x0000,0x0000,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0x0000,0x0000,0x0000},
    {0x0000,0x0000,0x7BEF,0xFFFF,0xFFFF,0x001F,0x001F,0xFFFF,0xFFFF,0x001F,0x001F,0xFFFF,0xFFFF,0x7BEF,0x0000,0x0000},
    {0x0000,0x7BEF,0xFFFF,0xFFFF,0x001F,0x001F,0x001F,0x001F,0x001F,0x001F,0x001F,0x001F,0xFFFF,0xFFFF,0x7BEF,0x0000},
    {0x0000,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0x001F,0x001F,0x7BEF,0x7BEF,0x001F,0x001F,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0x0000},
    {0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7BEF},
    {0x7BEF,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0xFFFF,0xFFFF,0xC618,0xC618,0xFFFF,0xFFFF,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0x7BEF},
    {0x0000,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xC618,0xC618,0xFFFF,0xFFFF,0xC618,0xC618,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0x0000},
    {0x0000,0x0000,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0x0000,0x0000},
    {0x0000,0x0000,0x0000,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xC618,0xC618,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0x0000,0x0000,0x0000},
    {0x0000,0x0000,0x0000,0x0000,0x7BEF,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0x7BEF,0x0000,0x0000,0x0000,0x0000},
    {0x0000,0x0000,0xF800,0xF800,0x0000,0x7BEF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7BEF,0x0000,0xF800,0xF800,0x0000,0x0000},
    {0x0000,0xF800,0xF800,0xF800,0x0000,0x0000,0x7BEF,0xFFFF,0xFFFF,0x7BEF,0x0000,0x0000,0xF800,0xF800,0xF800,0x0000},
    {0x0000,0x0000,0xF800,0xF800,0x0000,0x0000,0x0000,0x7BEF,0x7BEF,0x0000,0x0000,0x0000,0xF800,0xF800,0x0000,0x0000},
    {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000}
};

//spaceship functions
void addSpaceship() {
    if(spaceship_update_needed) {
        for (int i = 0; i < SHIP_SIZE; i++) {
            for (int j = 0; j < SHIP_SIZE; j++) {
                if (spaceship[i][j] != 0)
                    screen[spaceX + i][spaceY + j] = spaceship[i][j];
            }
        }
        spaceship_update_needed = false;
        display_update_needed = true;
    }
}

void removeSpaceship() {
    for (int i = 0; i < SHIP_SIZE; i++) {
        for (int j = 0; j < SHIP_SIZE; j++) {
            if (spaceship[i][j] != 0)
                screen[spaceX + i][spaceY + j] = background[spaceX + i][spaceY + j];
        }
    }
    display_update_needed = true;
}

void updateSpacePos(uint8_t joyX, uint8_t joyY) {
    int x = 3 - joyX;
    int y = joyY - 3;

    if (x || y) {
        x = (int)spaceX + x;
        y = (int)spaceY + y;

        if (x < 0)              x = 0;
        else if (x > MAX_SHIP_POS)     x = MAX_SHIP_POS;

        if (y < 0)              y = 0;
        else if (y > MAX_SHIP_POS)     y = MAX_SHIP_POS;

        if (x != spaceX || y != spaceY) {
            removeSpaceship();
            spaceX = (uint8_t)x;
            spaceY = (uint8_t)y;
            spaceship_update_needed = true;
        }
    }
}



//---------------------------------Everything related to enemy---------------------------------
//enemy variables
#define ENEMY_SIZE      8
#define ENEMY_BORDER    DISPLAY_SIZE/4
#define ENEMY_COUNT     12
#define ENEMY_BLOCKS    (DISPLAY_SIZE/ENEMY_COUNT)

uint8_t enemyPositions[ENEMY_COUNT][3] = {0};
uint8_t enemyX = 0;
uint8_t enemyY = 0;
const uint16_t enemyShip[ENEMY_SIZE][ENEMY_SIZE] = {
    {0x0000, 0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000},
    {0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000},
    {0xFFE0, 0xFFE0, 0xFD80, 0xFD80, 0xFD80, 0xFD80, 0xFFE0, 0xFFE0},
    {0xFFE0, 0xFD80, 0xFD80, 0x0000, 0x0000, 0xFD80, 0xFD80, 0xFFE0},
    {0xFFE0, 0xFD80, 0x0000, 0x0000, 0x0000, 0x0000, 0xFD80, 0xFFE0},
    {0xFFE0, 0xFD80, 0xFD80, 0xFD80, 0xFD80, 0xFD80, 0xFD80, 0xFFE0},
    {0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000},
    {0x0000, 0x0000, 0xFFE0, 0x0000, 0x0000, 0xFFE0, 0x0000, 0x0000}
};
uint8_t activeEnemyCount = 0;

//enemy functions
int randRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

void addEnemies() {
    activeEnemyCount = ENEMY_COUNT;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if(!enemyPositions[i][2]) {
            enemyPositions[i][0] = randRange(i*ENEMY_BLOCKS, i*ENEMY_BLOCKS);
            enemyPositions[i][1] = randRange(0, ENEMY_BORDER);
        }
    }

    display_update_needed = true;
}

void spawnEnemies() {
    for (int enemy = 0; enemy < activeEnemyCount; enemy++) {

        if(!enemyPositions[enemy][2]) {
            uint8_t enemyX = enemyPositions[enemy][0];
            uint8_t enemyY = enemyPositions[enemy][1];

            for (int i = 0; i < ENEMY_SIZE; i++) {
                for (int j = 0; j < ENEMY_SIZE; j++) {
                    if (enemyShip[i][j] != 0x0000) {
                        uint16_t x = enemyX + j;
                        uint16_t y = enemyY + i;
                        if (x < DISPLAY_SIZE && y < DISPLAY_SIZE) {
                            screen[y][x] = enemyShip[i][j];
                        }
                    }
                }
            }
            display_update_needed = true;
        }
    }
}

void removeEnemy() {
    for (int i = 0; i < ENEMY_SIZE; i++) {
        for (int j = 0; j < ENEMY_SIZE; j++) {
            if (enemyShip[i][j] != 0)
                screen[enemyX + i][enemyY + j] = background[enemyX + i][enemyY + j];
        }
    }
    display_update_needed = true;
}



//---------------------------------Everything related to projectile---------------------------------
#define PROJECTILE_SIZE 8

#define ENEMY_DIRECTION   1
#define PLAYER_DIRECTION -1

int projectiles[8][3];
int newProjectileIndex = 0;
bool canAddProject = true;

uint16_t projectileDesign[PROJECTILE_SIZE][PROJECTILE_SIZE] = {
    {BLACK, BLACK,  BLUE,   BLUE,   BLUE,   BLUE,  BLACK, BLACK},
    {BLACK, BLUE,   CYAN,   CYAN,   CYAN,   CYAN,  BLUE,  BLACK},
    {BLUE,  CYAN,   WHITE,  WHITE,  WHITE,  WHITE, CYAN,  BLUE},
    {BLUE,  CYAN,   WHITE,  YELLOW, YELLOW, WHITE, CYAN,  BLUE},
    {BLUE,  CYAN,   WHITE,  YELLOW, YELLOW, WHITE, CYAN,  BLUE},
    {BLUE,  CYAN,   WHITE,  WHITE,  WHITE,  WHITE, CYAN,  BLUE},
    {BLACK, BLUE,   CYAN,   CYAN,   CYAN,   CYAN,  BLUE,  BLACK},
    {BLACK, BLACK,  BLUE,   BLUE,   BLUE,   BLUE,  BLACK, BLACK}
};

void addProjectile() {
    int projectX = spaceX-PROJECTILE_SIZE;
    int projectY = spaceY+PROJECTILE_SIZE/2;
    
    for(int i = 0; i < PROJECTILE_SIZE; i++) {
        for(int j = 0; j < PROJECTILE_SIZE; j++) {
                if (projectileDesign[i][j] != 0 && projectX+i >= 0)
                    screen[projectX + i][projectY + j] = projectileDesign[i][j];
        }
    }
    projectiles[newProjectileIndex][0] = projectX;
    projectiles[newProjectileIndex][1] = projectY;
    projectiles[newProjectileIndex][2] = true;

    newProjectileIndex = (newProjectileIndex+1) % 8;
    
    display_update_needed = true;
}

void removeProjectile(int index, int projectX, int projectY) {
    for(int i = 0; i < PROJECTILE_SIZE; i++) {
        for(int j = 0; j < PROJECTILE_SIZE; j++) {
                if (projectileDesign[i][j] != 0 && projectX+i >= 0)
                    screen[projectX + i][projectY + j] = background[projectX + i][projectY + j];
        }
    }
}

void updateProjectile() {
    for(int i = 0; i < 8; i++) {
        if(projectiles[i][2]) {
            int projectX = projectiles[i][0];
            int projectY = projectiles[i][1];
            removeProjectile(i, projectX, projectY);
            projectX -= 2;

            if(projectX+PROJECTILE_SIZE > 0) {
                for(int k = 0; k < PROJECTILE_SIZE; k++) {
                    for(int j = 0; j < PROJECTILE_SIZE; j++) {
                            if (projectileDesign[k][j] != 0 && projectX+k >= 0)
                                screen[projectX + k][projectY + j] = projectileDesign[k][j];
                    }
                }
                projectiles[i][0] = projectX;
            }
            else {
                projectiles[i][2] = false;
            }
        }
    }
    display_update_needed = true;
}
