#include "displayDrawer.h"
#include <stdbool.h>


bool gameOver = false;

#define MEMORY_SIZE 128


//background and screen variable
uint16_t background[MEMORY_SIZE][DISPLAY_SIZE] = {0};
uint8_t backgroundStart = 0;
uint16_t screen[DISPLAY_SIZE][DISPLAY_SIZE];

//random function that gives values between min and max
int randRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

//generates the background using random function
void generateSpaceBackground() {
    for(int i = 0; i < MEMORY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {

            //the pixel is turn white (star pixel) only if the value is above 95
            //lowring this value from 95 to something small gives more stars
            if(randRange(0, DISPLAY_SIZE) > 95)   background[i][j] = WHITE;

            else background[i][j] = BLACK;
        }
    }
    //put the background in the screen
    for(int i = 0; i < DISPLAY_SIZE*DISPLAY_SIZE; i++) {
        screen[0][i] = background[0][i];
    }
}

void updateDisplay() {
    if(gameOver)
        for(int i = 0; i < DISPLAY_SIZE*DISPLAY_SIZE; i++) {
            screen[0][i] = 0x0000;
        }
    drawArray(0, 0, DISPLAY_SIZE, DISPLAY_SIZE, screen);
}

void moveBackground() {
    uint16_t oldStart = backgroundStart;
    backgroundStart = (backgroundStart - 1) % MEMORY_SIZE;
    for(int i = 0; i < DISPLAY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {
            if(background[(i+oldStart)%MEMORY_SIZE][j] != background[(i+backgroundStart)%MEMORY_SIZE][j]) {//only change the pixels that change in colors
                // screen[i][j] = background[(i+backgroundStart)%MEMORY_SIZE][j];
                drawPixel(i,j,background[(i+backgroundStart)%MEMORY_SIZE][j]);
            }
        }
    }
}


//---------------------------------Collision detection---------------------------------
//finds if the given corner (a,b) is within (c,d) and its other four corners
//it find the other four corners of (c,d) using its size cs and ds respectivly
bool intersects(int c, int d, int cs, int ds, int a, int b, int as, int bs) {
    bool x_axis = (c >= a) && (c <= a+as);
    bool y_axis = ((d >= b) && (d <= b+bs)) || ((d+ds >= b) && (d+ds <= b+bs));
    return x_axis && y_axis;
}



//---------------------------------Everything related to enemy---------------------------------
#define ENEMY_SIZE      8
#define ENEMY_BORDER    DISPLAY_SIZE/4//this is the border that enemies cannot cross making sure the user's spaceship has enough space to move around. essentially forces the enemies up the screen and won't let them come down
#define ENEMY_BLOCKS    (DISPLAY_SIZE/ENEMY_COUNT)//this makes blocks in which an enemy can spawn

uint8_t enemyPositions[ENEMY_COUNT][3] = {0};//this holds all the positions of the enemy in index 0 and 1. in index 2 it holds a boolean indicating they are alive or dead
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
uint8_t enemyIndex = 0;

//this is exactly the same as enemyShip except it holds enemy PROJECTILE info and not enemy info
int enemyProjectiles[ENEMY_COUNT][3];

//adds the given argument "count" number of enemies. If value is bigger than ENEMY_COUNT, it spawns ENEMY_COUNT number of enemies
void addEnemies(int count) {
    for (int i = 0; activeEnemyCount < ENEMY_COUNT && count >= 0; i++) {
        if(!enemyPositions[enemyIndex][2] && count >= 0) {//only work on the enemy if the enemy is not already alive
            enemyPositions[enemyIndex][0] = randRange(0, ENEMY_BORDER);//x axis value
            enemyPositions[enemyIndex][1] = enemyIndex*ENEMY_BLOCKS;//y axis value
            enemyPositions[enemyIndex][2] = true;//the enemy is now added and can be spawned
            activeEnemyCount++;
            count--;
        }
        enemyIndex = (enemyIndex+1) % ENEMY_COUNT;//helps find the enemy that is hit faster 
    }
}

//makes active enemies show up on the screen
void spawnEnemies() {
    for (int enemy = 0; enemy < ENEMY_COUNT; enemy++) {

        if(enemyPositions[enemy][2]) {
            //store the values for easy access
            uint8_t enemyX = enemyPositions[enemy][0];
            uint8_t enemyY = enemyPositions[enemy][1];

            for (int i = 0; i < ENEMY_SIZE; i++) {//add the enemy to the screen
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
            drawArray(enemyX, enemyY, ENEMY_SIZE, ENEMY_SIZE, screen);//send the screen variable to be drawn
            enemyPositions[enemy][2] = true;//enemy has now spawned
        }
    }
}

void removeEnemy(int index) {
    //store variable for easy access and understanding
    uint8_t enemyX = enemyPositions[index][0];
    uint8_t enemyY = enemyPositions[index][1];
    for(int i = 0; i < ENEMY_SIZE; i++) {//remove the enemy from the screen
        for (int j = 0; j < ENEMY_SIZE; j++) {
            if (enemyShip[i][j] != 0) {
                screen[enemyX + i][enemyY + j] = background[enemyX + i][enemyY + j];
            }
        }
    }
    drawArray(enemyX, enemyY, ENEMY_SIZE, ENEMY_SIZE, screen);//redraw the enemy block portion to remove from the screen
    activeEnemyCount--;
    enemyPositions[index][2] = false;//enemy no longer on the screen
    if(activeEnemyCount < 8) {//make sure enemy count never goes below 8
        addEnemies(1);
    }
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

//add enemy to the screen and draw it
void addSpaceship() {
    for (int i = 0; i < SHIP_SIZE_X; i++) {
        for (int j = 0; j < SHIP_SIZE_Y; j++) {
            if (spaceship[i][j] != 0)
                screen[spaceX + i][spaceY + j] = spaceship[i][j];
        }
    }
    drawArray(spaceX, spaceY, SHIP_SIZE_X, SHIP_SIZE_Y, screen);
}

//remove the enemy from the screen and redraw that area to show that it has been removed
void removeSpaceship() {
    for (int i = 0; i < SHIP_SIZE_X; i++) {
        for (int j = 0; j < SHIP_SIZE_Y; j++) {
            if (spaceship[i][j] != 0)
                screen[spaceX + i][spaceY + j] = background[spaceX + i][spaceY + j];
        }
    }
    drawArray(spaceX, spaceY, SHIP_SIZE_X, SHIP_SIZE_Y, screen);
}

//update the position and calculate the neccessory collisions and things needed to update the position and its logic
void updateSpacePos(uint8_t joyX, uint8_t joyY) {
    //calculate the value
    int x = 3 - joyX;
    int y = joyY - 3;

    if (x || y) {
        //calculate the correct direction
        x = (int)spaceX + x;
        y = (int)spaceY + y;

        //make sure the x and y values don't go over its max position and below its min
        if (x < 0)              x = 0;
        else if (x > MAX_SHIP_POS_X)     x = MAX_SHIP_POS_X;

        if (y < 0)              y = 0;
        else if (y > MAX_SHIP_POS_Y)     y = MAX_SHIP_POS_Y;

        //move the spaceship to its new position
        if (x != spaceX || y != spaceY) {
            removeSpaceship();
            spaceX = (uint8_t)x;
            spaceY = (uint8_t)y;
            for(int i = 0; i < ENEMY_COUNT; i++) {//see if the spaceship its an enemy. (spaceship and projectile collision taken care in projectile section)
                if(enemyPositions[i][2]) {
                    if(intersects(spaceX, spaceY, SHIP_SIZE_X, SHIP_SIZE_Y, enemyPositions[i][0], enemyPositions[i][1], ENEMY_SIZE, ENEMY_SIZE) ||
                        intersects(enemyPositions[i][0], enemyPositions[i][1], ENEMY_SIZE, ENEMY_SIZE, spaceX, spaceY, SHIP_SIZE_X, SHIP_SIZE_Y)) {//if spaceship is hit than game is over
                        gameOver = true;
                        return;
                    }
                }
            }
            addSpaceship();
        }
    }
}



//---------------------------------Everything related to user projectile---------------------------------
#define PROJECTILE_SIZE     4
#define PROJECTILE_NUM      12
#define PLAYER_DIRECTION    -2//increasing or decreasing this value (keeping it in negetive value) speeds the projectile or slows it down

int projectiles[PROJECTILE_NUM][3];//holds position of the projectile and if it exists or not
int newProjectileIndex = 0;//used to iterate through projectile to find the projectile space where a new projectile can be put faster
bool canAddProject = true;//makes sure that a projectile can be added or not

uint16_t projectileDesign[PROJECTILE_SIZE][PROJECTILE_SIZE] = {
    {BLACK,  CYAN, CYAN,  BLACK},
    {CYAN,  CYAN, CYAN,  CYAN},
    {CYAN,  CYAN, CYAN,  CYAN},
    {BLACK,  CYAN, CYAN,  BLACK},
};

//adds the projectle and draws it as well
void addProjectile() {
    if(canAddProject) {//only add if allowed to
        //get the position of the spaceship and calculate where the projectile should be accordingly
        int projectX = spaceX-PROJECTILE_SIZE;
        int projectY = spaceY+PROJECTILE_SIZE/2;
        
        for(int i = 0; i < PROJECTILE_SIZE; i++) {//add the projectile to the screen
            for(int j = 0; j < PROJECTILE_SIZE; j++) {
                if (projectileDesign[i][j] != 0 && projectX+i >= 0)
                    screen[projectX + i][projectY + j] = projectileDesign[i][j];
            }
        }
        drawArray(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, screen);
        //add the positions to projectile array
        projectiles[newProjectileIndex][0] = projectX;
        projectiles[newProjectileIndex][1] = projectY;
        projectiles[newProjectileIndex][2] = true;

        //if newProjectileIndex loops from 0 to PROJECTILE_NUM, it goes back to 0 indicating that we cannot add a projectile. Allowing only some number of projectiles to be active at once
        newProjectileIndex = (newProjectileIndex+1) % PROJECTILE_NUM;
        if(newProjectileIndex == 0) {
            canAddProject = false;
        }
    }
}

//removes the given projectile from the screen
void removeProjectile(int projectX, int projectY) {
    for(int i = 0; i < PROJECTILE_SIZE; i++) {
        for(int j = 0; j < PROJECTILE_SIZE; j++) {
            if (projectileDesign[i][j] != 0 && projectX+i >= 0) {
                screen[projectX + i][projectY + j] = background[projectX + i][projectY + j];
            }
        }
    }
    drawArray(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, screen);
}

//update the projectile position and calculate collision
void updateProjectile() {
    for(int i = 0; i < PROJECTILE_NUM; i++) {//run through each projectile
        if(projectiles[i][2]) {//only run if that projectile is active (exists)
            int projectX = projectiles[i][0];
            int projectY = projectiles[i][1];

            removeProjectile(projectX, projectY);//remove so we can either redraw it or just not have to remove it later if it hits something

            bool process = true;//this tells us if the projectile should be processed further depending on if the projectile hit something or not
            for (int j = 0; j < ENEMY_COUNT; j++) {//check all enemies to find the one that it hit
                //spaceship cannot possibly touch the projectiles due to its speed and projectile speed so no need to calculate it at all
                if (enemyPositions[j][2]) {//only check the active enemies
                    int ex = enemyPositions[j][0];
                    int ey = enemyPositions[j][1];

                    if (intersects(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, ex, ey, ENEMY_SIZE, ENEMY_SIZE)) {//check if we intersected
                        //if we intersected an enemy then we remove the enemy and not process the projectile any further
                        enemyPositions[j][2] = false;
                        removeEnemy(j);

                        for (int k = 0; k < ENEMY_SIZE; k++) {
                            for (int l = 0; l < ENEMY_SIZE; l++) {
                                screen[ex + k][ey + l] = background[ex + k][ey + l];
                            }
                        }
                        drawArray(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, screen);

                        projectiles[i][2] = false;
                        canAddProject = true;//since a projectile was distroyed we can now add another one

                        process = false;//no need to process this projectile any further
                        break;
                    }
                }
            }

            if(process) {//if we process then we never hit anything so just move to new position and add it to screen. Then redraw it
                projectX += PLAYER_DIRECTION;
                if(projectX+PROJECTILE_SIZE > 0) {//check if the projectile went over the screen
                    for(int k = 0; k < PROJECTILE_SIZE; k++) {
                        for(int j = 0; j < PROJECTILE_SIZE; j++) {
                            if (projectileDesign[k][j] != 0 && projectX+k >= 0) {
                                screen[projectX + k][projectY + j] = projectileDesign[k][j];
                            }
                        }
                    }
                    drawArray(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, screen);
                    projectiles[i][0] = projectX;
                }
                else {//no need to redraw if projectile went over the screen
                    projectiles[i][2] = false;
                    canAddProject = true;
                }
            }
        }
    }
}




//---------------------------------Everything related to enemy projectile---------------------------------
#define ENEMY_PROJECTILE_NUM  6//sets the limit to the number of enemy projectiles allowed on screen at once
#define ENEMY_DIRECTION       2

int newEnemyProjectile = 0;//used to keep track of the number of projectiles on screen
bool canAddEnemyProject = true;//puts a limit to the number of projectiles we can have

//add enemy projectiles to the screen
void addEnemyProjectile() {
    for(int i = 0; i < ENEMY_PROJECTILE_NUM && canAddEnemyProject; i++) {//run through all the projectiles as long as canAddEnemyProject is true
        int enemy = -1;
        for(int i = 0; i < ENEMY_PROJECTILE_NUM; i++) {//used to pick an enemy that is alive for giving it a projectile
            enemy = randRange(0,ENEMY_COUNT-1);//randomly pick an enemy index for giving enemy at that index a projectile

            if(!enemyProjectiles[enemy][2]) break;//if enemy doesn't have a projectile then we give it one

            else {//otherwise find an enemy manually to stop an infinite loop or random values that may never give us a projectile
                for(int i = 0; i < ENEMY_COUNT; i++) {
                    enemy = (enemy+1) % ENEMY_COUNT;
                    if(!enemyProjectiles[enemy][2]) break;
                }
            }
        }
        if(!enemyProjectiles[enemy][2] && enemyPositions[enemy][2]) {//run if projectile isn't added and that enemy is alive
            int projectX = enemyPositions[enemy][0]+ENEMY_SIZE;
            int projectY = enemyPositions[enemy][1]+PROJECTILE_SIZE/2;
            
            for(int i = 0; i < PROJECTILE_SIZE; i++) {//add the projectile to screen
                for(int j = 0; j < PROJECTILE_SIZE; j++) {
                    if (projectileDesign[i][j] != 0 && projectX+i >= 0) {
                        screen[projectX + i][projectY + j] = projectileDesign[i][j];
                    }
                }
            }
            drawArray(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, screen);
            //set its positions in the array
            enemyProjectiles[enemy][0] = projectX;
            enemyProjectiles[enemy][1] = projectY;
            enemyProjectiles[enemy][2] = true;

            //make sure there are only certain number of projectile on the screen
            newEnemyProjectile = newEnemyProjectile + 1;
            if(newEnemyProjectile == ENEMY_PROJECTILE_NUM) {
                canAddEnemyProject = false;
            }
        }
    }
}

//remove the given projectile from the screen and update the screen
void removeEnemyProjectile(int projectX, int projectY) {
    for(int i = 0; i < PROJECTILE_SIZE; i++) {
        for(int j = 0; j < PROJECTILE_SIZE; j++) {
            if (projectileDesign[i][j] != 0 && projectX+i >= 0) {
                screen[projectX + i][projectY + j] = background[projectX + i][projectY + j];
            }
        }
    }
    drawArray(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, screen);
}

void updateEnemyProjectile() {
    for(int i = 0; i < ENEMY_COUNT; i++) {
        if(enemyProjectiles[i][2]) {//run only if the projectile exists
            int projectX = enemyProjectiles[i][0];
            int projectY = enemyProjectiles[i][1];

            removeEnemyProjectile(projectX, projectY);

            bool process = true;

            if (intersects(spaceX, spaceY, SHIP_SIZE_X, SHIP_SIZE_Y, projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE) ||
                intersects(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, spaceX, spaceY, SHIP_SIZE_X, SHIP_SIZE_Y)) {//see if the projectile hits the spaceship
                //there are two intersects since intesects checks if a corner is inside a box. this checks for both corners of the projectile being inside the spaceship
                removeSpaceship();

                //if the spaceship is hit then the game is over
                enemyProjectiles[i][2] = false;
                canAddEnemyProject = false;
                newEnemyProjectile--;
                gameOver = true;

                process = false;
            }

            if(process) {
                projectX += ENEMY_DIRECTION;
                if(projectX+PROJECTILE_SIZE < DISPLAY_SIZE) {//process the projectile further only if its still in the screen
                    for(int k = 0; k < PROJECTILE_SIZE; k++) {
                        for(int j = 0; j < PROJECTILE_SIZE; j++) {
                            if (projectileDesign[k][j] != 0 && projectX+k < DISPLAY_SIZE) {
                                screen[projectX + k][projectY + j] = projectileDesign[k][j];
                            }
                        }
                    }
                    enemyProjectiles[i][0] = projectX;
                    drawArray(projectX, projectY, PROJECTILE_SIZE, PROJECTILE_SIZE, screen);
                }
                else {
                    enemyProjectiles[i][2] = false;
                    canAddEnemyProject = true;
                    newEnemyProjectile--;
                }
            }
        }
    }
}

bool game_over() { return gameOver; }

//reset the variables to their default to reset the game
void resetGame() {
    spaceX = DEFAULT_SHIP_X;
    spaceY = DEFAULT_SHIP_Y;

    for(int i = 0; i < ENEMY_COUNT; i++) {
        enemyPositions[i][2] = 0;
        enemyProjectiles[i][2] = 0;
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
    backgroundStart = 0;

    generateSpaceBackground();
    for(int i = 0; i < DISPLAY_SIZE; i++) {
        for(int j = 0; j < DISPLAY_SIZE; j++) {
            screen[i][j] = background[i][j];
        }
    }
    addSpaceship();
    addEnemies(ENEMY_COUNT);
    updateDisplay();
}