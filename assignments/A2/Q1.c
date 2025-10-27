#include "sam.h"
#include <stdlib.h>
#include "../display.h"

#define FILTER_LEN 100
#define THRESHOLD 500
#define CENTER DISPLAY_SIZE/2
#define SAMP_SIZE 10
#define MAX 2700
#define MIN 15

volatile uint32_t msTicks = 0;
int samples[SAMP_SIZE];
int sampleIndex = 0;
int sampleSize = 0;
int sum = 0;
int drop = (MAX-MIN)/DISPLAY_SIZE;
int currSize = 0;
int prevSize = 0;

void SysTick_Handler() {
    msTicks++;
}

int getADC() {
    ADC0_REGS->ADC_SWTRIG = ADC_SWTRIG_START_Msk;
    while (!(ADC0_REGS->ADC_INTFLAG & ADC_INTFLAG_RESRDY_Msk));
    int val = ADC0_REGS->ADC_RESULT;
    return val;
}

void getSample() {
    int value = getADC();
    
    if(sampleSize < SAMP_SIZE) {
        sum += value;
        sampleSize++;
    }
    else {
        sum = (sum - samples[sampleIndex]) + value;
    }

    samples[sampleIndex] = value;
    sampleIndex = (sampleIndex+1) % SAMP_SIZE;
}

int calculateBoxSize() {
    int val = sum/SAMP_SIZE;
    val = (val-MIN)/drop;
    if(val < 0) {
        val = 0;
    }
    if(val > DISPLAY_SIZE) {
        val = DISPLAY_SIZE;
    }
    return val/2;
}

void drawBox() {
    //erase previous lines
    for (int i = 0; i < DISPLAY_SIZE; i++) {
        displayDrawPixel(i, CENTER+prevSize-1, BLACK);
        displayDrawPixel(i, CENTER-prevSize, BLACK);
        displayDrawPixel(CENTER-prevSize, i, BLACK);
        displayDrawPixel(CENTER+prevSize-1, i, BLACK);
    }

    //draw new lines
    for (int i = 0; i < DISPLAY_SIZE; i++) {
        displayDrawPixel(i, CENTER+currSize-1, BLUE);
        displayDrawPixel(i, CENTER-currSize, WHITE);
        displayDrawPixel(CENTER-currSize, i, BLUE);
        displayDrawPixel(CENTER+currSize-1, i, WHITE);
    }
}

void ADC_Hall_Init() {
    PORT_REGS->GROUP[1].PORT_DIRCLR = PORT_PB03;
    PORT_REGS->GROUP[1].PORT_PINCFG[3] = PORT_PINCFG_PMUXEN_Msk;
    PORT_REGS->GROUP[1].PORT_PMUX[1] |= PORT_PMUX_PMUXO_B;

    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA06;
    PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA06;

    MCLK_REGS->MCLK_APBDMASK |= MCLK_APBDMASK_ADC0_Msk;
    GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_GENEN_Msk | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_DIV(8);
    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK0) == GCLK_SYNCBUSY_GENCTRL_GCLK0);
    GCLK_REGS->GCLK_PCHCTRL[ADC0_GCLK_ID] = GCLK_PCHCTRL_CHEN_Msk | GCLK_PCHCTRL_GEN_GCLK0;

    SUPC_REGS->SUPC_VREF = SUPC_VREF_SEL_2V5;

    ADC0_REGS->ADC_INPUTCTRL = ADC_INPUTCTRL_MUXPOS_AIN15;
    while (ADC0_REGS->ADC_SYNCBUSY);

    ADC0_REGS->ADC_CTRLA |= ADC_CTRLA_ENABLE_Msk;
    while (ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_ENABLE_Msk);
}

int main() {
    displayInit();
    ADC_Hall_Init();
    SysTick_Config(48000);

    int prevSum = 0;

    while (1) {
        if (sum > prevSum + THRESHOLD || sum + THRESHOLD < prevSum) {
            currSize = calculateBoxSize();
            drawBox();
            prevSum = sum;
            prevSize = currSize;
        }
        getSample();
    }
}
