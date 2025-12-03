#include "displayDrawer.h"
#include <stdbool.h>


//display variables
#define MEMORY_SIZE 128

bool gameOver = false;

bool display_update_needed = true;
uint16_t background[MEMORY_SIZE][DISPLAY_SIZE] = {0};
uint8_t backgroundStart = 0;
uint16_t screen[DISPLAY_SIZE][DISPLAY_SIZE];


int randRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

void generateSpaceBackground() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        for (int j = 0; j < DISPLAY_SIZE; j++) {
            if(randRange(0, DISPLAY_SIZE) > 95)   background[i][j] = WHITE;
            else            background[i][j] = BLACK;
        }
    }
    for(int i = 0; i < DISPLAY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {
            screen[i][j] = background[i][j];
        }
    }
}

void updateDisplay() {
    if (display_update_needed && !gameOver) {
        drawArray(0, DISPLAY_SIZE, 0, DISPLAY_SIZE, *screen);
        display_update_needed = false;
    }
}

void moveBackground() {
    backgroundStart = (backgroundStart - 1) % MEMORY_SIZE;
    for(int i = 0; i < DISPLAY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {
            screen[i][j] = background[(i+backgroundStart)%MEMORY_SIZE][j];
        }
    }
}


//---------------------------------Collision detection---------------------------------
bool intersects(int c, int d, int cs, int ds, int a, int b, int as, int bs) {
    bool x_axis = (c >= a) && (c <= a+as);
    bool y_axis = ((d >= b) && (d <= b+bs)) || ((d+ds >= b) && (d+ds <= b+bs));
    return x_axis && y_axis;
}



//---------------------------------Everything related to spaceship---------------------------------
#define SHIP_SIZE_X       13
#define SHIP_SIZE_Y       8
#define MAX_SHIP_POS_X    (DISPLAY_SIZE - SHIP_SIZE_X)
#define MAX_SHIP_POS_Y    (DISPLAY_SIZE - SHIP_SIZE_Y)
#define DEFAULT_SHIP_X    DISPLAY_SIZE - SHIP_SIZE_X * 1.5
#define DEFAULT_SHIP_Y    DISPLAY_SIZE / 2 - SHIP_SIZE_Y / 2

uint8_t spaceX = DEFAULT_SHIP_X;
uint8_t spaceY = DEFAULT_SHIP_Y;
const uint16_t spaceship[SHIP_SIZE_X][SHIP_SIZE_Y] = {
    {BLACK,BLACK,BLACK,LIGHT_GRAY,LIGHT_GRAY,BLACK,BLACK,BLACK},
    {BLACK,BLACK,LIGHT_GRAY,WHITE,WHITE,LIGHT_GRAY,BLACK,BLACK},
    {BLACK,LIGHT_GRAY,WHITE,CYAN,CYAN,WHITE,LIGHT_GRAY,BLACK},
    {BLACK,WHITE,CYAN,BLUE,BLUE,CYAN,WHITE,BLACK},
    {GRAY,WHITE,CYAN,BLUE,BLUE,CYAN,WHITE,GRAY},
    {LIGHT_GRAY,WHITE,CYAN,CYAN,CYAN,CYAN,WHITE,LIGHT_GRAY},
    {WHITE,WHITE,WHITE,CYAN,CYAN,WHITE,WHITE,WHITE},
    {WHITE,LIGHT_GRAY,WHITE,GREEN,GREEN,WHITE,LIGHT_GRAY,WHITE},
    {LIGHT_GRAY,LIGHT_GRAY,GREEN,GREEN,GREEN,GREEN,LIGHT_GRAY,LIGHT_GRAY},
    {GRAY,GREEN,WHITE,RED,RED,WHITE,GREEN,GRAY},
    {GREEN,GREEN,BLACK,RED,RED,BLACK,GREEN,GREEN},
    {GREEN,BLACK,BLACK,RED,RED,BLACK,BLACK,GREEN},
    {BLACK,BLACK,BLACK,RED,RED,BLACK,BLACK,BLACK},
};


void addSpaceship() {
    for (int i = 0; i < SHIP_SIZE_X; i++) {
        for (int j = 0; j < SHIP_SIZE_Y; j++) {
            if (spaceship[i][j] != 0)
                screen[spaceX + i][spaceY + j] = spaceship[i][j];
        }
    }
    display_update_needed = true;
}

void removeSpaceship() {
    for (int i = 0; i < SHIP_SIZE_X; i++) {
        for (int j = 0; j < SHIP_SIZE_Y; j++) {
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
        else if (x > MAX_SHIP_POS_X)     x = MAX_SHIP_POS_X;

        if (y < 0)              y = 0;
        else if (y > MAX_SHIP_POS_Y)     y = MAX_SHIP_POS_Y;

        if (x != spaceX || y != spaceY) {
            removeSpaceship();
            spaceX = (uint8_t)x;
            spaceY = (uint8_t)y;
            addSpaceship();
        }
    }
}



//---------------------------------Everything related to enemy---------------------------------
#define ENEMY_SIZE      8
#define ENEMY_BORDER    DISPLAY_SIZE/4
#define ENEMY_COUNT     12
#define ENEMY_BLOCKS    (DISPLAY_SIZE/ENEMY_COUNT)

uint8_t enemyPositions[ENEMY_COUNT][3] = {0};
const uint16_t enemyShip[ENEMY_SIZE][ENEMY_SIZE] = {
    {BLACK, BLACK, YELLOW, YELLOW, YELLOW, YELLOW, BLACK, BLACK},
    {BLACK, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, BLACK},
    {YELLOW, YELLOW, GOLDEN, GOLDEN, GOLDEN, GOLDEN, YELLOW, YELLOW},
    {YELLOW, GOLDEN, GOLDEN, BLACK, BLACK, GOLDEN, GOLDEN, YELLOW},
    {YELLOW, GOLDEN, BLACK, BLACK, BLACK, BLACK, GOLDEN, YELLOW},
    {YELLOW, GOLDEN, GOLDEN, GOLDEN, GOLDEN, GOLDEN, GOLDEN, YELLOW},
    {BLACK, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, BLACK},
    {BLACK, BLACK, YELLOW, BLACK, BLACK, YELLOW, BLACK, BLACK}
};
uint8_t activeEnemyCount = 0;


void addEnemies() {
    activeEnemyCount = ENEMY_COUNT;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        if(!enemyPositions[i][2]) {
            enemyPositions[i][0] = randRange(0, ENEMY_BORDER);
            enemyPositions[i][1] = randRange(i*ENEMY_BLOCKS, i*ENEMY_BLOCKS);
            enemyPositions[i][2] = true;
        }
    }

    display_update_needed = true;
}

void spawnEnemies() {
    for (int enemy = 0; enemy < activeEnemyCount; enemy++) {

        if(enemyPositions[enemy][2]) {
            uint8_t enemyX = enemyPositions[enemy][0];
            uint8_t enemyY = enemyPositions[enemy][1];

            for (int i = 0; i < ENEMY_SIZE; i++) {
                for (int j = 0; j < ENEMY_SIZE; j++) {
                    if (enemyShip[i][j] != BLACK) {
                        uint16_t x = enemyX + i;
                        uint16_t y = enemyY + j;
                        if (x < DISPLAY_SIZE && y < DISPLAY_SIZE) {
                            screen[x][y] = enemyShip[i][j];
                        }
                    }
                }
            }
            enemyPositions[enemy][2] = true;
            display_update_needed = true;
        }
    }
}

void removeEnemy(int i) {
    uint8_t enemyX = enemyPositions[i][0];
    uint8_t enemyY = enemyPositions[i][1];
    for(int i = 0; i < ENEMY_SIZE; i++)
        for (int j = 0; j < ENEMY_SIZE; j++) {
            if (enemyShip[i][j] != 0)
                screen[enemyX + i][enemyY + j] = background[enemyX + i][enemyY + j];
        }
    display_update_needed = true;
}



//---------------------------------Everything related to projectile---------------------------------
#define PROJECTILE_SIZE     4
#define PROJECTILE_NUM      12
#define PLAYER_DIRECTION    -5

int projectiles[PROJECTILE_NUM][3];
int newProjectileIndex = 0;
bool canAddProject = true;

uint16_t projectileDesign[PROJECTILE_SIZE][PROJECTILE_SIZE] = {
    {BLACK,  CYAN, CYAN,  BLACK},
    {CYAN,  CYAN, CYAN,  CYAN},
    {CYAN,  CYAN, CYAN,  CYAN},
    {BLACK,  CYAN, CYAN,  BLACK},
};

void addProjectile() {
    if(canAddProject) {
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

        newProjectileIndex = (newProjectileIndex+1) % PROJECTILE_NUM;
        if(newProjectileIndex == 0) {
            canAddProject = false;
        }
        
        display_update_needed = true;
    }
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
    for(int i = 0; i < PROJECTILE_NUM; i++) {
        if(projectiles[i][2]) {
            int projectX = projectiles[i][0];
            int projectY = projectiles[i][1];

            removeProjectile(i, projectX, projectY);

            bool process = true;
            for (int j = 0; j < ENEMY_COUNT; j++) {
                if (enemyPositions[j][2]) {
                    int ex = enemyPositions[j][0];
                    int ey = enemyPositions[j][1];

                    if (intersects(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, ex, ey, ENEMY_SIZE, ENEMY_SIZE)) {
                        enemyPositions[j][2] = false;
                        removeEnemy(j);

                        for (int k = 0; k < ENEMY_SIZE; k++)
                            for (int l = 0; l < ENEMY_SIZE; l++)
                                screen[ey + k][ex + l] = background[ey + k][ex + l];

                        projectiles[i][2] = false;
                        canAddProject = true;

                        process = false;
                        break;
                    }
                }
            }

            if(process) {
                projectX += PLAYER_DIRECTION;
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
                    canAddProject = true;
                }
            }
        }
    }
    display_update_needed = true;
}

//--------------------------------------------------------------------------------------------------

#define ENEMY_PROJECTILE_NUM  6
#define ENEMY_DIRECTION       3

int enemyProjectiles[ENEMY_COUNT][3];
int newEnemyProjectile = 0;
bool canAddEnemyProject = true;

void addEnemyProjectile() {
    for(int i = 0; i < ENEMY_PROJECTILE_NUM && canAddEnemyProject; i++) {
        int enemy = -1;
        for(int i = 0; i < ENEMY_PROJECTILE_NUM; i++) {
            enemy = randRange(0,ENEMY_COUNT-1);
            if(!enemyProjectiles[enemy][2]) break;
        }
        if(!enemyProjectiles[enemy][2]) {
            int projectX = enemyPositions[enemy][0]+ENEMY_SIZE;
            int projectY = enemyPositions[enemy][1]+PROJECTILE_SIZE/2;
            
            for(int i = 0; i < PROJECTILE_SIZE; i++) {
                for(int j = 0; j < PROJECTILE_SIZE; j++) {
                        if (projectileDesign[i][j] != 0 && projectX+i >= 0)
                            screen[projectX + i][projectY + j] = projectileDesign[i][j];
                }
            }
            enemyProjectiles[enemy][0] = projectX;
            enemyProjectiles[enemy][1] = projectY;
            enemyProjectiles[enemy][2] = true;

            newEnemyProjectile = newEnemyProjectile + 1;
            if(newEnemyProjectile == ENEMY_PROJECTILE_NUM) {
                canAddEnemyProject = false;
            }
            
            display_update_needed = true;
        }
    }
}

void removeEnemyProjectile(int projectX, int projectY) {
    for(int i = 0; i < PROJECTILE_SIZE; i++) {
        for(int j = 0; j < PROJECTILE_SIZE; j++) {
                if (projectileDesign[i][j] != 0 && projectX+i >= 0)
                    screen[projectX + i][projectY + j] = background[projectX + i][projectY + j];
        }
    }
}

void updateEnemyProjectile() {
    for(int i = 0; i < ENEMY_COUNT; i++) {
        if(enemyProjectiles[i][2]) {
            int projectX = enemyProjectiles[i][0];
            int projectY = enemyProjectiles[i][1];

            removeEnemyProjectile(projectX, projectY);

            bool process = true;

            if (intersects(spaceX, spaceY, SHIP_SIZE_X, SHIP_SIZE_Y, projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE)) {
                removeSpaceship();

                enemyProjectiles[i][2] = false;
                canAddEnemyProject = false;
                newEnemyProjectile--;
                gameOver = true;

                process = false;
            }

            if(process) {
                projectX += ENEMY_DIRECTION;
                if(projectX+PROJECTILE_SIZE < DISPLAY_SIZE) {
                    for(int k = 0; k < PROJECTILE_SIZE; k++) {
                        for(int j = 0; j < PROJECTILE_SIZE; j++) {
                            if (projectileDesign[k][j] != 0 && projectX+k < DISPLAY_SIZE)
                                screen[projectX + k][projectY + j] = projectileDesign[k][j];
                        }
                    }
                    enemyProjectiles[i][0] = projectX;
                }
                else {
                    enemyProjectiles[i][2] = false;
                    canAddEnemyProject = true;
                    newEnemyProjectile--;
                }
            }
        }
    }
    display_update_needed = true;
}


void resetGame() {
    spaceX = DEFAULT_SHIP_X;
    spaceY = DEFAULT_SHIP_Y;

    for(int i = 0; i < ENEMY_COUNT; i++) {
        enemyPositions[i][2] = 0;
        enemyProjectiles[i][2] = 0;
        projectiles[i][2] = 0;
    }

    for(int i = 0; i < PROJECTILE_NUM; i++) {
        projectiles[i][2] = 0;
    }
    newEnemyProjectile = 0;
    newProjectileIndex = 0;
    canAddEnemyProject = true;
    canAddProject = true;
    activeEnemyCount = 0;
    gameOver = false;
    display_update_needed = true;

    for(int i = 0; i < DISPLAY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {
        screen[i][j] = background[i][j];
        }
    }

    addSpaceship();
    addEnemies();
}