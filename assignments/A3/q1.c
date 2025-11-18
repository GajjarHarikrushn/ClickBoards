#include "sam.h"
#include <stdbool.h>
#include "../display.h"

#define GCLK_DIV 48000

#define START_BYTE 255/10
#define DOT_BYTE 255/50
#define DASH_BYTE 255/75
#define END_BYTE 255/25


uint32_t msCount = 0;
bool sendData = false;
bool clicked = false;
uint32_t clicked_at = 0;
uint32_t released_at = 0;
uint16_t displayBits = 0;

void updateOutput(uint16_t dutyCycle) {
  TC1_REGS->COUNT16.TC_CC[1] = dutyCycle;
}

void TCC0_OTHER_Handler() {
  if ((TCC0_REGS->TCC_INTFLAG & TCC_INTFLAG_OVF_Msk) == TCC_INTFLAG_OVF_Msk)
  {
    TCC0_REGS->TCC_INTFLAG |= TCC_INTFLAG_OVF_Msk;
  }
}

void TCC0_MC0_Handler() {
  nextSample = TCC0_REGS->TCC_CC[0] + (ovFlaw*256);
  uint32_t value = nextSample-prevSample;
  sFlaw++;
  displayDrawDigit(DISPLAY_SIZE/2 - FONT_SIZE/2, (DISPLAY_SIZE/2) - 20, BLUE, sFlaw/1000);
  displayDrawDigit(DISPLAY_SIZE/2 - FONT_SIZE/2, (DISPLAY_SIZE/2) - 8, BLUE, (sFlaw/100)%10);
  displayDrawDigit(DISPLAY_SIZE/2 - FONT_SIZE/2, (DISPLAY_SIZE/2) + 4, BLUE, (sFlaw/10)%10);
  displayDrawDigit(DISPLAY_SIZE/2 - FONT_SIZE/2, (DISPLAY_SIZE/2) + 16, BLUE, sFlaw%10);
  prevSample = nextSample;
}

void SysTick_Handler() {
  msCount++;
}

void PB14_init() {
  PORT_REGS->GROUP[1].PORT_DIRCLR = PORT_PB14;
  PORT_REGS->GROUP[1].PORT_PINCFG[14] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
  PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB14;
  PORT_REGS->GROUP[1].PORT_PMUX[7] = PORT_PMUX_PMUXE_A;

  MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_EVSYS_Msk;
  EVSYS_REGS->EVSYS_USER[EVENT_ID_USER_TCC0_MC_0] = 0x01;
  EVSYS_REGS->CHANNEL[0].EVSYS_CHANNEL = EVSYS_CHANNEL_EVGEN(0x20) | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;

  EIC_REGS->EIC_CONFIG[1] = EIC_CONFIG_SENSE6_BOTH;
  EIC_REGS->EIC_EVCTRL = PORT_PB14;
  EIC_REGS->EIC_CTRLA = EIC_CTRLA_CKSEL_CLK_ULP32K | EIC_CTRLA_ENABLE_Msk;

  GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_DIV(GCLK_DIV) | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_GENEN_Msk;
  while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK0) == GCLK_SYNCBUSY_GENCTRL_GCLK0);

  GCLK_REGS->GCLK_PCHCTRL[TCC0_GCLK_ID] = GCLK_PCHCTRL_GEN(0) | GCLK_PCHCTRL_CHEN_Msk;
  while ((GCLK_REGS->GCLK_PCHCTRL[TCC0_GCLK_ID] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk);

  MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_TCC0_Msk;

  TCC0_REGS->TCC_PER = 0xFF;
  TCC0_REGS->TCC_EVCTRL = TCC_EVCTRL_MCEI0_Msk;
  TCC0_REGS->TCC_INTENSET = TCC_INTENSET_MC0_Msk | TCC_INTENSET_OVF_Msk;
  TCC0_REGS->TCC_CTRLA = TCC_CTRLA_CPTEN0_Msk | TCC_CTRLA_ENABLE_Msk;

  NVIC_EnableIRQ(TCC0_MC0_IRQn);
  NVIC_EnableIRQ(TCC0_OTHER_IRQn);
}

void PA11_init() {
  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA11;
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA11;
  PORT_REGS->GROUP[0].PORT_PINCFG[11] |= PORT_PINCFG_PMUXEN_Msk;
  PORT_REGS->GROUP[0].PORT_PMUX[5] |= PORT_PMUX_PMUXO_E;

  GCLK_REGS->GCLK_GENCTRL[1] = GCLK_GENCTRL_DIV(GCLK_DIV) | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_GENEN_Msk;
  while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK1) == GCLK_SYNCBUSY_GENCTRL_GCLK1);

  GCLK_REGS->GCLK_PCHCTRL[TC1_GCLK_ID] = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN_Msk;
  while ((GCLK_REGS->GCLK_PCHCTRL[9] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk);

  MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_TC1_Msk;

  TC1_REGS->COUNT16.TC_CTRLA = TC_CTRLA_MODE_COUNT16;
  TC1_REGS->COUNT16.TC_WAVE = TC_WAVE_WAVEGEN_NPWM;
  TC1_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;
}

void PA15_init() {
  PORT_REGS->GROUP[0].PORT_DIRCLR = PORT_PA15;
  PORT_REGS->GROUP[0].PORT_PINCFG[15] |= PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;
}

void logic() {
  if((PORT_REGS->GROUP[0].PORT_IN & PORT_PA15) && !clicked) {// if button is pressed but clicked is false
    clicked_at = msCount;
    clicked = true;
  }

  if(!(PORT_REGS->GROUP[0].PORT_IN & PORT_PA15) && clicked) {// if button is no longer but clicked is true
    released_at = msCount;
    clicked = false;
  }

  if(released_at-clicked_at >= 1000 && !sendData) {// send start byte since we are now sending data
    sendData = true;
    updateOutput(START_BYTE);
  }

  
}

int main() {
  NVIC_EnableIRQ(SysTick_IRQn);
  SysTick_Config(48000);
  displayInit();

  PA11_init();
  PB14_init();

  updateOutput(0);

  __enable_irq();

  displayErase();
  while(1) {
    __WFI();
    logic();
  }
}