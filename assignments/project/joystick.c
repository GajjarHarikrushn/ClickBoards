#include "joystick.h"

#define CS_PIN          PORT_PB05
#define SET_CS_LOW      PORT_REGS->GROUP[1].PORT_OUTCLR = CS_PIN
#define SET_CS_HIGH     PORT_REGS->GROUP[1].PORT_OUTSET = CS_PIN

void activateJoy() {
    spiActivate(1,1,0,1);
    SET_CS_LOW;
}

void deactivateJoy() {
    spiDeactivate();
    SET_CS_HIGH;
}


//read the given channel which sets the axis
uint8_t readAxis(uint8_t ch) {
    uint8_t cmd = 0x6 | (ch >> 2);
    uint8_t cmd2 = (ch & 0x3) << 6;

    activateJoy();

    spiWriteByte(cmd);
    spiWriteByte(cmd2);
    spiReadByte();//these bits are read to remove them since they are not needed at all
    uint8_t result = spiReadByte() & 0x7;//these bits are read since they are needed but only the last three bits are required. others are too jumpy and not needed

    deactivateJoy();
    return result;
}

void joystickInit() {
    spiInit();
    PORT_REGS->GROUP[1].PORT_DIRSET = CS_PIN;
    SET_CS_HIGH;
}