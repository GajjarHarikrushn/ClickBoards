#include "sam.h"
#include "spi.h"

#define CMD_COL 0x15
#define CMD_ROW 0x75
#define CMD_RAM 0x5C
#define CMD_DISPLAYOFF 0xAE
#define CMD_DISPLAYON 0xAF
#define CMD_REMAP 0xA0
#define CMD_STARTLINE 0xA1
#define CMD_DISPLAYMODE 0xA6
#define CMD_SETMULTIPLEX 0xCA
#define CMD_SETMASTER 0xC7
#define CMD_SETCONTRAST 0xC1
#define CMD_UNLOCK 0xFD

#define DC_PORT   PORT_REGS->GROUP[1].PORT_OUTTGL
#define DC_PIN    PORT_PB09

void writeCmd(uint8_t cmd) {
    DC_PORT = DC_PIN;
    spiWriteByte(cmd);
    DC_PORT = DC_PIN;
}

void writeColor(uint16_t c) {
    spiWriteByte(c >> 8);
    spiWriteByte(c & 0xFF);
}

void displayInit() {
    PORT_REGS->GROUP[1].PORT_DIRSET = DC_PIN;
    PORT_REGS->GROUP[1].PORT_OUTSET = DC_PIN;

    spiInit();
    spiActivate(1,0,0,1);
}

void drawPixel(uint8_t x, uint8_t y, uint16_t color)
{
    writeCmd(CMD_SETCOLUMN);
    spiWriteByte(x);
    spiWriteByte(x);

    writeCmd(CMD_SETROW);
    spiWriteByte(y);
    spiWriteByte(y);

    writeCmd(CMD_WRITERAM);
    writeColor(color);
}

// ----------------------------
// MAIN
// ----------------------------
int main()
{
    displayInit();

    while (1) {
        drawPixel(0, 0, 0xFFFF);
        drawPixel(50, 50, 0xFFFF);
    };
}
