#include "spi.h"

#define DISPLAY_SIZE  96

#define BLACK       0x0000
#define WHITE       0xFFFF
#define BLUE        0x001F
#define CYAN        0x07FF
#define YELLOW      0xFFE0

void displayInit();
void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawArray(uint8_t x, uint8_t X_size, uint8_t y, uint8_t Y_size, uint16_t *color);