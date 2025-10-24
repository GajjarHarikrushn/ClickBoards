#include "sam.h"
#include "../display.h"
#include "../logger.h"
#include "../dcc_stdio.h"
#include <assert.h>
#include <stdbool.h>

// 16-bit timer and a 48MHz clock, means we need 48000 ticks to get to 1ms
// since we can't change TOP, change count using an offset starting value 
#define MS_TICKS_OFFSET (65536-48000)

#define EXTINT15_MASK 0x8000

volatile uint8_t speedIX = 0;

void timerInit()
{
  // have to enable the interrupt line in the system level REG
  NVIC_EnableIRQ(TC0_IRQn);

  // have to enable the peripheral clocks I need via the generic and main clocks
  // see page 156 of data sheet for GCLK array offsets
  GCLK_REGS->GCLK_PCHCTRL[9] = GCLK_PCHCTRL_CHEN_Msk;
  while ((GCLK_REGS->GCLK_PCHCTRL[9] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    ;/* Wait for synchronization */

  MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_TC0_Msk;

  // we can only modify TOP in 8-bit mode. since we're using 16-bit mode we get accuracy by modifying COUNT
  TC0_REGS->COUNT16.TC_COUNT = MS_TICKS_OFFSET;

  // enable the overflow interrupt 
  TC0_REGS->COUNT16.TC_INTENSET = TC_INTENSET_OVF_Msk;

  // start the timer
  TC0_REGS->COUNT16.TC_CTRLA = TC_CTRLA_ENABLE_Msk;
}

// ISR for all timer 0 interrupts; assuming overflow since that's all we've enabled
void TC0_Handler()
{
  #define NUM_SPEEDS 5
  static uint16_t speeds[NUM_SPEEDS] = {5, 10, 25, 50, 100};
  static bool dirRight = true;
  static uint8_t x = 0;
  static uint8_t y = (DISPLAY_SIZE/2) - 1;
  static uint16_t count = 0;

  // set the offset every time!
  TC0_REGS->COUNT16.TC_COUNT = MS_TICKS_OFFSET;

  // clear the interrupt!
  TC0_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_OVF_Msk;

  count++;
  if (count >= speeds[speedIX])
  {
    displayReplacePixel(x, y, RED);

    if (dirRight)
      x++;
    else
      x--;
          
    // reset direction when we hit the last pixel
    if (x == DISPLAY_SIZE)
    {
      dirRight = false;
    }
    else if (x == 0)
    {
      dirRight = true;
    }
    
    count = 0;
  }
}

void buttonInit()
{
  // button input on PA15, processed as an external interrupt
  PORT_REGS->GROUP[0].PORT_DIRCLR = PORT_PA15;
  PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
  PORT_REGS->GROUP[0].PORT_PMUX[7] = PORT_PMUX_PMUXO_A;

  // must activate pull-up on the processor's button
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;

  // have to enable the interrupt line in the system level REG
  NVIC_EnableIRQ(EIC_EXTINT_15_IRQn);

  MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;

  EIC_REGS->EIC_CONFIG[1] = EIC_CONFIG_SENSE7_RISE;
  EIC_REGS->EIC_INTENSET = EXTINT15_MASK;
  EIC_REGS->EIC_CTRLA = EIC_CTRLA_CKSEL_CLK_ULP32K | EIC_CTRLA_ENABLE_Msk;
}

// ISR for external interrupt 15
void EIC_EXTINT_15_Handler()
{
  // clear the interrupt! and update the speed
  EIC_REGS->EIC_INTFLAG = EXTINT15_MASK;
  speedIX = (speedIX + 1) % NUM_SPEEDS;
}

int main(void)
{
#ifndef NDEBUG
  for (int i=0; i<100000; i++)
  ;
#endif

  // sleep to idle (wake on interrupts)
  PM_REGS->PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_IDLE;
  
  displayInit();

  timerInit();
  buttonInit();

  // we want interrupts!
  __enable_irq();

  // sleep until we have an interrupt
  while (1) 
  {
    __WFI();
  }
}

