#include "sam.h"
#include "displayDrawer.h"
#include "joystick.h"
#include "../logger.h"

#define wait(x) for(int i = 0; i < x; i++)

uint32_t msCount = 0;

void EIC_EXTINT_15_Handler() {
    EIC_REGS->EIC_INTFLAG = PORT_PA15;
    addProjectile();
}

void EIC_EXTINT_14_Handler() {
    EIC_REGS->EIC_INTFLAG = PORT_PB14;
    resetGame();
}

void buttonInit() {
    PORT_REGS->GROUP[0].PORT_DIRCLR = PORT_PA15;
    PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
    PORT_REGS->GROUP[0].PORT_PMUX[7] = PORT_PMUX_PMUXO_A;
    PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;

    PORT_REGS->GROUP[1].PORT_DIRCLR = PORT_PB14;
    PORT_REGS->GROUP[1].PORT_PINCFG[14] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
    PORT_REGS->GROUP[1].PORT_PMUX[7] = PORT_PMUX_PMUXE_A;
    PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB14;


    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;

    EIC_REGS->EIC_CONFIG[1] |= EIC_CONFIG_SENSE7_RISE;
    EIC_REGS->EIC_INTENSET |= PORT_PA15;

    EIC_REGS->EIC_CONFIG[1] |= EIC_CONFIG_SENSE6_FALL;
    EIC_REGS->EIC_INTENSET |= PORT_PB14;

    NVIC_EnableIRQ(EIC_EXTINT_15_IRQn);
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
    int start = msCount;
        updateSpacePos(readAxis(X_AXIS),readAxis(Y_AXIS));
    logMsg("%2.2f", msCount-start);
        moveBackground();
    logMsg("%2.2f", msCount-start);
        updateProjectile();
    logMsg("%2.2f", msCount-start);
        spawnEnemies();
    logMsg("%2.2f", msCount-start);
        addSpaceship();
    logMsg("%2.2f", msCount-start);
        updateDisplay();
    logMsg("%2.2f", msCount-start);


    while(1) {
        updateSpacePos(readAxis(X_AXIS),readAxis(Y_AXIS));
        moveBackground();
        updateProjectile();
        updateEnemyProjectile();
        addEnemyProjectile();
        spawnEnemies();
        addSpaceship();
        updateDisplay();
    }
}