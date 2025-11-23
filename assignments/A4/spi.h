#pragma once

#include "sam.h"
#include <stddef.h>
#include <stdbool.h>

// The SPI is running off of a 48MHz clock. Base all BAUD settings on this clock frequency.
// This unit configures and uses generic clock 3.

void spiInit();
void spiActivate(uint8_t baud, bool rx, bool idleHigh, bool leadingSample);
void spiDeactivate();

void spiFlushBuffer();
uint8_t spiReadByte();
void spiWriteByte(uint8_t value);
