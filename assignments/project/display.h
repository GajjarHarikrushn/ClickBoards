#include "spi.h"

#define DISPLAY_SIZE  96

void displayInit();
void drawPixel(uint8_t x, uint8_t y, uint16_t color);
void drawArray(uint8_t x, uint8_t y, uint16_t *color, uint8_t xSize, uint8_t ySize);