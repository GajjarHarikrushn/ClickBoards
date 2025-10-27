#include "sam.h"
#include "../display.h"
#include "stdbool.h"

#define BUTTON1_INDEX 2
#define BUTTON1_MASK PORT_PA02
#define BUTTON2_INDEX 7
#define BUTTON2_MASK PORT_PA07
#define FAN_SPEED_CHANGE 5
#define HYST_ON_LIMIT 0
#define BUTTON_PRESSED(port_in, mask) (port_in & mask)

volatile int fanSpeed = 100; // this represents percentage
volatile uint32_t msCount = 0;

typedef struct BUTTON {
  uint32_t last_clicked;
  uint32_t released_at;
  uint32_t click_time;
  uint32_t button;
  uint32_t pressCount;
  uint32_t releaseCount;
  bool on;
}button;


void SysTick_Handler() {
    msCount++;
    TC1_REGS->COUNT16.TC_CC[1] = (65535 * fanSpeed) / 100;
    while (TC1_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC1_Msk);
}

void updateButton(uint32_t port_in, button *key) {
    if(BUTTON_PRESSED(port_in, key->button)) {
        key->pressCount++;
        key->releaseCount = 0;

        if(key->pressCount >= HYST_ON_LIMIT) {
        if(!key->on) {
            key->on = true;
            key->last_clicked = msCount;
            key->click_time = 0;
        }
        }
    } else {
        key->pressCount = 0;
        key->releaseCount++;

        if(key->releaseCount >= HYST_ON_LIMIT) {
        if(key->on) {
            key->on = false;
            key->click_time = msCount-key->last_clicked;
            key->released_at = msCount;
            key->last_clicked = 0;
        }
        }
    }
}

void button_setup(int index, int mask) {
    PORT_REGS->GROUP[0].PORT_DIRCLR |= mask;
    PORT_REGS->GROUP[0].PORT_PINCFG[index] |= PORT_PINCFG_PMUXEN_Msk | PORT_PINCFG_PULLEN_Msk;
    PORT_REGS->GROUP[0].PORT_OUTSET |= mask;
}

void setFan() {
    MCLK_REGS->MCLK_APBAMASK |= MCLK_APBAMASK_TC1_Msk;

    GCLK_REGS->GCLK_GENCTRL[1] = GCLK_GENCTRL_GENEN_Msk | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_DIV(8);
    while (GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL_GCLK1);

    GCLK_REGS->GCLK_PCHCTRL[TC1_GCLK_ID] = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN_Msk;
    while (!(GCLK_REGS->GCLK_PCHCTRL[TC1_GCLK_ID] & GCLK_PCHCTRL_CHEN_Msk));

    PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA11;
    PORT_REGS->GROUP[0].PORT_PINCFG[11] |= PORT_PINCFG_PMUXEN_Msk;
    PORT_REGS->GROUP[0].PORT_PMUX[5] |= PORT_PMUX_PMUXO_E;

    TC1_REGS->COUNT16.TC_WAVE = TC_WAVE_WAVEGEN_NPWM;

    TC1_REGS->COUNT16.TC_CC[0] = 65535;
    while (TC1_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC0_Msk);
    TC1_REGS->COUNT16.TC_CC[1] = 65535;
    while (TC1_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC1_Msk);


    TC1_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;
    while (TC1_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE_Msk);
}

void logic(button *key1, button *key2) {
    if(!key1->on && !key2->on) {//if key1 is off and key2 is off
        if(key1->released_at > key2->released_at) {
            fanSpeed = (fanSpeed-FAN_SPEED_CHANGE >= 5) ? fanSpeed-FAN_SPEED_CHANGE : fanSpeed;
            key1->released_at = 0;
        }
        if(key1->released_at < key2->released_at) {
            fanSpeed = (fanSpeed+FAN_SPEED_CHANGE <= 100) ? fanSpeed+FAN_SPEED_CHANGE : fanSpeed;
            key2->released_at = 0;
        }
    }
    if(key1->on && !key2->on) {//if key1 is on while key2 is off
        if(msCount-key1->last_clicked > 1000 && (msCount-key1->last_clicked) % 500 == 0){//if key has been clicked over 1 second and time between each fanSpeed change is 500 ms
            fanSpeed = (fanSpeed-FAN_SPEED_CHANGE >= 5) ? fanSpeed-FAN_SPEED_CHANGE : fanSpeed;
        }
    }
    else if(!key1->on && key2->on) {//if key1 is off while key2 is on
        if(msCount-key2->last_clicked > 1000 && (msCount-key2->last_clicked) % 500 == 0){//if key has been clicked over 1 second and time between each fanSpeed change is 500 ms
            fanSpeed = (fanSpeed+FAN_SPEED_CHANGE <= 100) ? fanSpeed+FAN_SPEED_CHANGE : fanSpeed;
        }
    }
}

int main(void)
{
    setFan();

    SysTick_Config(48000);
    NVIC_EnableIRQ(SysTick_IRQn);

    button_setup(BUTTON1_INDEX, BUTTON1_MASK);
    button_setup(BUTTON2_INDEX, BUTTON2_MASK);
    
    displayInit();

    button button1 = {0,0,0,BUTTON1_MASK};
    button button2 = {0,0,0,BUTTON2_MASK};

    while (1) {
        __WFI();

        updateButton(PORT_REGS->GROUP[0].PORT_IN, &button1);
        updateButton(PORT_REGS->GROUP[0].PORT_IN, &button2);

        logic(&button1, &button2);
    }
}