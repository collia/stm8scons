
#include "stm8s.h"
#include "stm8s_gpio.h"


void debug_init() {
        GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);
}


void debug_blink_fast_forever() {
    int i;
    while(1) {
        GPIO_WriteReverse(GPIOC, GPIO_PIN_6);
        for(i = 0; i < 1600; i++);
    }
}

void debug_blink_slow_forever() {
    int i;
    while(1) {
        GPIO_WriteReverse(GPIOC, GPIO_PIN_6);
        for(i = 0; i < 24000; i++);
    }
}

void debug_blink_1_sec() {
    int i, j;
    
    GPIO_WriteReverse(GPIOC, GPIO_PIN_6);
    for(i = 0; i < 24000; i++)
        for(j = 0; j < 10; j++);
}

