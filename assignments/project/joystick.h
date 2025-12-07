#pragma once

#include "sam.h"
#include "spi.h"

#define X_AXIS  1
#define Y_AXIS  0

void joystickInit();
uint8_t readAxis(uint8_t ch);