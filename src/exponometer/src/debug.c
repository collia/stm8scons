
#include "stm8s.h"
#include "stm8s_gpio.h"

#define DEBUG_PORT GPIOA
#define DEBUG_PIN GPIO_PIN_2

void debug_init() {
#ifdef DEBUG
    GPIO_Init(DEBUG_PORT, DEBUG_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_WriteHigh(DEBUG_PORT, DEBUG_PIN);
    //GPIO_WriteReverse(DEBUG_PORT, DEBUG_PIN);    
#endif
}


void debug_blink_fast_forever() {
#ifdef oDEBUG
    int i;
    while(1) {
        GPIO_WriteReverse(DEBUG_PORT, DEBUG_PIN);
        for(i = 0; i < 1600; i++);
    }
#endif
}

void debug_blink_slow_forever() {
#ifdef oDEBUG
    int i;
    while(1) {
        GPIO_WriteReverse(DEBUG_PORT, DEBUG_PIN);
        for(i = 0; i < 24000; i++);
    }
#endif
}

void debug_blink_1_sec() {
#ifdef oDEBUG
    int i, j;
    
    GPIO_WriteReverse(DEBUG_PORT, DEBUG_PIN);
    for(i = 0; i < 16000; i++)
        for(j = 0; j < 10; j++);
#endif
}


void debug_blink_once() {
#ifdef DEBUG    
    GPIO_WriteReverse(DEBUG_PORT, DEBUG_PIN);
#endif
}


void debug_set_once(uint8_t en) {
#ifdef DEBUG
    if(en) {
        GPIO_WriteHigh(DEBUG_PORT, DEBUG_PIN);
    } else {
        GPIO_WriteLow(DEBUG_PORT, DEBUG_PIN);
    }
#endif
}
