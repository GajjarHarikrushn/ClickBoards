#include "sam.h"
#include <stdbool.h>
#include "../display.h"

#define GCLK_DIV_TC 48
#define GCLK_DIV_TCC_EIC 48

#define START_BYTE 5000
#define DOT_BYTE 15000
#define DASH_BYTE 20000
#define END_BYTE 10000
#define EMPTY_BYTE 25000
#define WAIT_BEFORE_DESPLAY 250
#define WAIT_BEFORE_TRANSFER 50
#define ERROR_RATE 1000
#define DASH_LED 1000
#define DOT_LED 250


uint32_t msCount = 0;
bool sendData = false;

//click variables
bool clicked = false;
uint32_t clicked_at = 0;
uint32_t released_at = 0;

//receive variables
uint16_t rvPos = 0;
uint16_t rvLen = 0;
uint16_t rvBits = 0;
bool receiving = false;
bool receiveEmpty = false;

//transmit variables
uint16_t txBits = 0;
uint16_t txLen  = 0;
uint16_t txPos = 0;
bool transmitting = false;
int sendStart = 0;

//display variables
int dash = 0;
bool on = false;
bool delayOn = false;
int delayCount = 0;
uint16_t bit;

void TC1_Handler() {
  if (TC1_REGS->COUNT16.TC_INTFLAG & TC_INTFLAG_OVF_Msk) {
    TC1_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_OVF_Msk;

    if (transmitting) {
      uint16_t next;
      if (txPos == 0 && sendStart < 2) {
        next = START_BYTE;
        sendStart++;
      }           // Start pulse
      else if (txPos < txLen) {                     // Data bits
        next = ((txBits >> (txLen - txPos - 1)) & 1) ? DASH_BYTE : DOT_BYTE;
        txPos++;
      }
      else if (txPos == txLen){
        next = END_BYTE; // End pulse
        txPos++;
      } else {
        transmitting = false;
        txBits = 0;
        txLen = 0;
        next = 0;
        sendStart = 0;
      }

      TC1_REGS->COUNT16.TC_CC[1] = next;
      TC1_REGS->COUNT16.TC_CTRLBSET = TC_CTRLBSET_CMD_UPDATE;

    }
  }
}

void TCC0_OTHER_Handler() {
  if ((TCC0_REGS->TCC_INTFLAG & TCC_INTFLAG_OVF_Msk) == TCC_INTFLAG_OVF_Msk)
  {
    TCC0_REGS->TCC_INTFLAG |= TCC_INTFLAG_OVF_Msk;
  }
}

void TCC0_MC0_Handler(void) {
    while (TCC0_REGS->TCC_SYNCBUSY & TCC_SYNCBUSY_CC0_Msk);
    uint16_t duration = TCC0_REGS->TCC_CC[0];

    if (duration >= START_BYTE - ERROR_RATE && duration <= START_BYTE + ERROR_RATE) {
        receiving = true;
    } else if (duration >= END_BYTE - ERROR_RATE && duration <= END_BYTE + ERROR_RATE) {
        receiving = false;
    } else if (receiving) {
        if (duration >= DOT_BYTE - ERROR_RATE && duration <= DOT_BYTE + ERROR_RATE) {
          rvBits = (rvBits << 1) | 0;
          rvLen++;
        }
        else if (duration >= DASH_BYTE - ERROR_RATE && duration <= DASH_BYTE + ERROR_RATE) {
          rvBits = (rvBits << 1) | 1;
          rvLen++;
        }
    }
}

void SysTick_Handler() {
  msCount++;
}

void PB14_init() {
  PORT_REGS->GROUP[1].PORT_DIRCLR = PORT_PB14;
  PORT_REGS->GROUP[1].PORT_PINCFG[14] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
  PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB14;
  PORT_REGS->GROUP[1].PORT_PMUX[7] = PORT_PMUX_PMUXE_A;
}

void PA11_init() {
  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA11;
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA11;
  PORT_REGS->GROUP[0].PORT_PINCFG[11] |= PORT_PINCFG_PMUXEN_Msk;
  PORT_REGS->GROUP[0].PORT_PMUX[5] |= PORT_PMUX_PMUXO_E;
}

void GCLK_init() {
  GCLK_REGS->GCLK_GENCTRL[1] = GCLK_GENCTRL_DIV(GCLK_DIV_TC) | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_GENEN_Msk;
  while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK1) == GCLK_SYNCBUSY_GENCTRL_GCLK1);

  GCLK_REGS->GCLK_PCHCTRL[TC1_GCLK_ID] = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN_Msk;
  while ((GCLK_REGS->GCLK_PCHCTRL[TC1_GCLK_ID] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk);

  GCLK_REGS->GCLK_PCHCTRL[TCC0_GCLK_ID] = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN_Msk;
  while ((GCLK_REGS->GCLK_PCHCTRL[TCC0_GCLK_ID] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk);

  GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN_Msk;
  while ((GCLK_REGS->GCLK_PCHCTRL[EIC_GCLK_ID] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk);
}

void EVSYS_init() {
  MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_EVSYS_Msk;
  EVSYS_REGS->EVSYS_USER[EVENT_ID_USER_TCC0_MC_0] = 0x01;
  EVSYS_REGS->CHANNEL[0].EVSYS_CHANNEL = EVSYS_CHANNEL_EVGEN(0x20) | EVSYS_CHANNEL_PATH_ASYNCHRONOUS;
}

void EIC_init() {
  EIC_REGS->EIC_CONFIG[1] = EIC_CONFIG_SENSE6_BOTH;
  EIC_REGS->EIC_EVCTRL = PORT_PB14;
  EIC_REGS->EIC_CTRLA = EIC_CTRLA_ENABLE_Msk;
}

void TC1_init() {
  MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_TC1_Msk;

  TC1_REGS->COUNT16.TC_CTRLA = TC_CTRLA_MODE_COUNT16;
  TC1_REGS->COUNT16.TC_WAVE = TC_WAVE_WAVEGEN_NPWM;
  TC1_REGS->COUNT16.TC_CTRLBSET = TC_CTRLBSET_LUPD_Msk;
  TC1_REGS->COUNT16.TC_INTENSET = TC_INTENSET_OVF_Msk;
  TC1_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;

  NVIC_EnableIRQ(TC1_IRQn);
}

void TCC0_init() {
  MCLK_REGS->MCLK_APBBMASK |= MCLK_APBBMASK_TCC0_Msk;

  TCC0_REGS->TCC_EVCTRL = TCC_EVCTRL_MCEI0_Msk;
  TCC0_REGS->TCC_INTENSET = TCC_INTENSET_MC0_Msk | TCC_INTENSET_OVF_Msk;
  TCC0_REGS->TCC_CTRLA = TCC_CTRLA_CPTEN0_Msk | TCC_CTRLA_ENABLE_Msk;

  NVIC_EnableIRQ(TCC0_MC0_IRQn);
  NVIC_EnableIRQ(TCC0_OTHER_IRQn);
}

void PA15_init() {
  PORT_REGS->GROUP[0].PORT_DIRCLR = PORT_PA15;
  PORT_REGS->GROUP[0].PORT_PINCFG[15] |= PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;
}

void logic() {
  bool pressed = !(PORT_REGS->GROUP[0].PORT_IN & PORT_PA15);

  if (pressed && !clicked) {
    clicked = true;
    clicked_at = msCount;
  }

  if (!pressed && clicked) {
    clicked = false;
    released_at = msCount;

    uint32_t duration = released_at - clicked_at;

    if (duration <= 250) {
      if (txLen < 16) {
        txBits = (txBits << 1) | 0;   // dot
        txLen++;
      }
    } else {
      if (txLen < 16) {
        txBits = (txBits << 1) | 1;   // dash
        txLen++;
      }
    }
  }

  if (txLen > 0 && !transmitting) {
    if (msCount - released_at >= 1000) {
      transmitting = true;
      txPos = 0;
    }
  }
}

void led_logic() {
  if(delayOn) {
    if(msCount-delayCount == WAIT_BEFORE_DESPLAY)
      delayOn = false;
  }
  else {
    if(!on) {
      if (rvPos < rvLen) {
        dash = msCount;
        bit = (rvBits >> (rvLen - rvPos - 1)) & 1;
        if(bit) {
          if ((msCount - dash) < DASH_LED && !on) {
            PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA14;  // turn ON
            on = true;
            rvPos++;
          }
        }
        else {
          if ((msCount - dash) < DOT_LED && !on) {
            PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA14;  // turn ON
            on = true;
            rvPos++;
          }
        }
      }
    }

    if ((msCount - dash) >= DASH_LED && on && bit) {
      PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA14;  // turn OFF
      on = false;
      delayOn = true;
      delayCount = msCount;
    }


    if ((msCount - dash) >= DOT_LED && on && !bit) {
      PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA14;  // turn OFF
      on = false;
      delayOn = true;
      delayCount = msCount;
    }
  }
}

int main() {
  NVIC_EnableIRQ(SysTick_IRQn);
  SysTick_Config(48000);

  PA11_init();
  PB14_init();
  GCLK_init();
  TC1_init();
  TCC0_init();
  EVSYS_init();
  EIC_init();
  PA15_init();

  __enable_irq();

  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA14;
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA14;
  while(1) {
    logic();
    led_logic();
  }
}