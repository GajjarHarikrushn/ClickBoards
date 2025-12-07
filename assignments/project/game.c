#include "sam.h"
#include "displayDrawer.h"
#include "joystick.h"


#define NUM_TASKS 8
typedef void (*task_func)(void);

typedef struct {
    uint32_t period;
    uint32_t last_run; 
    task_func func;
} Task;

volatile uint32_t msCount = 0;

void EIC_EXTINT_14_Handler() {
    EIC_REGS->EIC_INTFLAG = PORT_PB14;
    resetGame();
    msCount = 0;
}

void buttonInit() {
    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA15;
    PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
    PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;

    PORT_REGS->GROUP[1].PORT_DIRCLR = PORT_PB14;
    PORT_REGS->GROUP[1].PORT_PINCFG[14] = PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
    PORT_REGS->GROUP[1].PORT_PMUX[7] = PORT_PMUX_PMUXE_A;
    PORT_REGS->GROUP[1].PORT_OUTSET = PORT_PB14;

    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_EIC_Msk;

    EIC_REGS->EIC_CONFIG[1] |= EIC_CONFIG_SENSE6_FALL;
    EIC_REGS->EIC_INTENSET = PORT_PB14;

    NVIC_EnableIRQ(EIC_EXTINT_14_IRQn);

    EIC_REGS->EIC_CTRLA = EIC_CTRLA_CKSEL_CLK_ULP32K | EIC_CTRLA_ENABLE_Msk;
}

void SysTick_Handler() {
    msCount++;
}

void updateSpacePosition() {
    updateSpacePos(readAxis(X_AXIS),readAxis(Y_AXIS));
}

void shoot() {
    if(!(PORT_REGS->GROUP[0].PORT_IN & PORT_PA15)) {
        addProjectile();
    }
}

int main() {
    SysTick_Config(48000);
    NVIC_EnableIRQ(SysTick_IRQn);
    displayInit();
    joystickInit();
    generateSpaceBackground();
    addEnemies(ENEMY_COUNT);
    spawnEnemies();
    buttonInit();

    //these are the tasks that will be used
    //{period, last executed at, function to run}
    Task tasks[NUM_TASKS] = {
        {20, 0, updateProjectile},
        {50, 0, addSpaceship},
        {50, 0, updateSpacePosition},
        {50, 0, addEnemyProjectile},
        {50, 0, updateEnemyProjectile},
        {100, 0, moveBackground},
        {100, 0, spawnEnemies},
        {100, 0, shoot}
    };
    
    while(1) {
        EIC_REGS->EIC_INTENCLR = PORT_PB14;//disable the reset button interrupt to stop reset during the execution
        if(!game_over()) {
            for(int i = 0; i < NUM_TASKS; i++) {
                if(msCount - tasks[i].last_run >= tasks[i].period) {
                    tasks[i].last_run = msCount;
                    tasks[i].func();
                }
            }
        }
        else {
            updateDisplay();
        }
        EIC_REGS->EIC_INTENSET = PORT_PB14;//enable the reset button interrupt to allow for reset after the execution
    }
}