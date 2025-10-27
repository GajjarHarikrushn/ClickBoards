#include "sam.h"
#include "../dcc_stdio.h"
#include <assert.h>

// setup our heartbeat to be 1ms: we overflow at 1ms intervals with a 48MHz clock
// uses the SysTicks unit so that we get reliable debugging (timer stops on breakpoints)
#define MS_TICKS 48000UL

// number of milliseconds between LED flashes
#define LED_FLASH_MS    100

// NOTE: this overflows every ~50 days, so I'm not going to care here...
// from a1q2 on, we will use this for all scheduling done in main()
volatile uint32_t msCount = 0;

void heartInit()
{
  // have to enable the interrupt line in the system level REG
  NVIC_EnableIRQ(SysTick_IRQn);

  SysTick_Config(MS_TICKS);
}

// Fires every 1ms
void SysTick_Handler()
{
  msCount++;
}

int main(void)
{
#ifndef NDEBUG
  for (int i=0; i<100000; i++)
  ;
  // include the following line if you want to simulate the 'standard' stop on entry behaviour
  // WARNING: this will always breakpoint, even when not in a debugger. Meaning that your code will never execute if outside a debugger.
  //__BKPT(0);
#endif

  // NOTE: the silkscreen on the curiosity board is WRONG! it's PB4 and PB5, NOT PA4 and PA5, right beside the processor

  // see the header files within include/component for register definitions, which align with the data sheet for the processor
  // e.g. port.h contains the masks and definitions for manipulating gpio

  // LED output
  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA14;
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA14;

  // sleep to idle (wake on interrupts)
  PM_REGS->PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_IDLE;
  
  heartInit();

  // we want interrupts!
  __enable_irq();

  // sleep until we have an interrupt
  while (1) 
  {
    __WFI();

    if ((msCount % LED_FLASH_MS) == 0)
    {
      PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA14;
    }
  }
}
