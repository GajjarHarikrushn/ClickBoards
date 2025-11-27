#include "sam.h"
#include "display.h"

#define PLAYER_SIZE 16
#define wait(x) for(int i = 0; i < x; i++)


uint16_t screen[DISPLAY_SIZE][DISPLAY_SIZE]; // definition here

int main() {
    displayInit();
    uint16_t x = 0;
    while(1) {
        for(int i = 0; i < DISPLAY_SIZE; i++) {
            for(int j = 0; j < DISPLAY_SIZE; j++) {
                drawPixel(i,j,x);
                x++;
            }
        }
    }
    // for(int i = 0; i < 16; i++) {
    //     for(int i = 0; i < 6*6; i++)
    //         drawPixel()
    //     x += 15;
    // }
}