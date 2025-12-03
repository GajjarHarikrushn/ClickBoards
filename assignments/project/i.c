#include "sam.h"
#include "displayDrawer.h"
#include "joystick.h"
#include "../logger.h"

uint32_t msCount = 0;

void EIC_EXTINT_14_Handler() {
    EIC_REGS->EIC_INTFLAG = PORT_PB14;
    resetGame();
}

void buttonInit() {
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA15;
    PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;
    PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;

    PORT_REGS->GROUP[1].PORT_DIRCLR = PORT_PB14;
    PORT_REGS->GROUP[1].PORT_PINCFG[14] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
    PORT_REGS->GROUP[1].PORT_PMUX[7] = PORT_PMUX_PMUXE_A;
    PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB14;

    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;

    EIC_REGS->EIC_CONFIG[1] |= EIC_CONFIG_SENSE6_FALL;
    EIC_REGS->EIC_INTENSET |= PORT_PB14;

    NVIC_EnableIRQ(EIC_EXTINT_14_IRQn);

    EIC_REGS->EIC_CTRLA = EIC_CTRLA_CKSEL_CLK_ULP32K | EIC_CTRLA_ENABLE_Msk;
}

void SysTick_Handler() {
    msCount++;
}

int main() {
    SysTick_Config(48000);
    NVIC_EnableIRQ(SysTick_IRQn);
    logInit();
    displayInit();
    joystickInit();
    generateSpaceBackground();
    addEnemies(ENEMY_COUNT);
    spawnEnemies();
    buttonInit();

    while(1) {
        updateSpacePos(readAxis(X_AXIS),readAxis(Y_AXIS));
        moveBackground();
        updateProjectile();
        if(!(PORT_REGS->GROUP[0].PORT_IN & PORT_PA15))
            addProjectile();
        updateEnemyProjectile();
        addEnemyProjectile();
        spawnEnemies();
        addSpaceship();
        updateDisplay();
    }
}