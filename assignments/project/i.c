#include "sam.h"
#include "displayDrawer.h"
#include "joystick.h"

#define wait(x) for(int i = 0; i < x; i++)

void EIC_EXTINT_15_Handler() {
  // clear the interrupt! and update the speed
  EIC_REGS->EIC_INTFLAG = PORT_PA15;
  addProjectile();
}

void buttonInit() {
  PORT_REGS->GROUP[0].PORT_DIRCLR = PORT_PA15;
  PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
  PORT_REGS->GROUP[0].PORT_PMUX[7] = PORT_PMUX_PMUXO_A;
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;

  NVIC_EnableIRQ(EIC_EXTINT_15_IRQn);

  MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;

  EIC_REGS->EIC_CONFIG[1] = EIC_CONFIG_SENSE7_RISE;
  EIC_REGS->EIC_INTENSET = PORT_PA15;
  EIC_REGS->EIC_CTRLA = EIC_CTRLA_CKSEL_CLK_ULP32K | EIC_CTRLA_ENABLE_Msk;
}

int main() {
    displayInit();
    joystickInit();
    generateSpaceBackground();
    // addEnemies();
    // spawnEnemies();
    buttonInit();
    while(1) {
        updateSpacePos(readAxis(X_AXIS),readAxis(Y_AXIS));
        addSpaceship();
        updateDisplay();
        updateProjectile();
    }
}