#include "sam.h"
#include "display.h"

#define wait(x) for(int i = 0; i < x; i++)

#define CMD_COL                 0x15
#define CMD_ROW                 0x75
#define CMD_RAM                 0x5C
#define CMD_DISPLAYUNLOCK       0xFD
#define CMD_REMAP               0xA0
#define CMD_STARTLINE           0xA1
#define CMD_OFFSET              0xA2
#define CMD_DISPLAYMODE         0xA6
#define CMD_DISPLAYON           0xAF
#define CMD_DISPLAYENHANCE      0xB2

#define DC_PIN      PORT_PB09
#define RST_PIN     PORT_PB06
#define CS_PIN      PORT_PB04
#define RW_PIN      PORT_PB08
#define EN_PIN      PORT_PB07
#define low(pin)    PORT_REGS->GROUP[1].PORT_OUTCLR = pin
#define high(pin)   PORT_REGS->GROUP[1].PORT_OUTSET = pin

void activate() {
    spiActivate(1,0,0,1);
    low(CS_PIN);
}

void deactivate() {
    spiDeactivate();
    high(CS_PIN);
}

void cmd(uint8_t c) {
    low(DC_PIN);
    spiWriteByte(c);
}

void data(uint8_t d) {
    high(DC_PIN);
    spiWriteByte(d);
}

void displayInit(void) {
    spiInit();

    PORT_REGS->GROUP[1].PORT_DIRSET = DC_PIN | RST_PIN | CS_PIN | EN_PIN;
    high(DC_PIN | RST_PIN | EN_PIN);
    low(CS_PIN);
    activate();

    cmd(CMD_DISPLAYON);
    cmd(CMD_DISPLAYMODE);
    cmd(CMD_STARTLINE);         data(0x00);
    cmd(CMD_DISPLAYUNLOCK);     data(0x12);
    cmd(CMD_DISPLAYUNLOCK);     data(0xB1);
    cmd(CMD_OFFSET);            data(0x00);
    cmd(CMD_REMAP);             data(0x32);
    cmd(CMD_DISPLAYENHANCE);    data(0x00); data(0x00);

    // Clear screen
    cmd(CMD_COL); data(0x00); data(DISPLAY_SIZE-1);
    cmd(CMD_ROW); data(0x00); data(DISPLAY_SIZE-1);
    cmd(CMD_RAM);
    for (int i = 0; i < 127*127; i++) {
        data(0x00); data(0x00); // black pixel
    }

    deactivate();
    wait(1000000);
}

void drawPixel(uint8_t x, uint8_t y, uint16_t color) {

    activate();

    cmd(CMD_COL); data(x+16); data(x+16);
    cmd(CMD_ROW); data(y); data(y);
    cmd(CMD_RAM);

    data(color >> 8);
    data(color & 0xFF);

    deactivate();
}

void drawArray(uint8_t x, uint8_t y, uint16_t *color, uint8_t xSize, uint8_t ySize) {
    activate();

    cmd(CMD_COL); data(x+16); data(xSize+16);
    cmd(CMD_ROW); data(y); data(ySize);
    cmd(CMD_RAM);

    for(int i = 0; i < xSize*ySize; i++) {
        data(color[i] >> 8);
        data(color[i] & 0xFF);
    }

    deactivate();
}
