#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "exponometer.h"

void print_f(uint8_t f) {
    if(f & F_NUM_POINT) {
        printf("f%2.1f", ((float)(f & (~F_NUM_POINT)))/10);
    } else {
        printf("f%2"PRIu8, f);
    }
}


void print_speed(uint16_t s) {
    if((s & (~(EXP_SPEED_MINUTES|EXP_SPEED_SECONDS))) == 0) {
        printf(" ------");
    } else if(s & EXP_SPEED_MINUTES) {
        printf("  %4"PRIu16"m", (s & (~EXP_SPEED_MINUTES)));
    } else if (s & EXP_SPEED_SECONDS) {
        printf("  %4"PRIu16" ", (s & (~EXP_SPEED_SECONDS)));
    } else {
        printf("1/%-4"PRIu16" ", s);
    }
}


int main(void) {
    uint16_t speed;
    uint8_t apt;
    for(int16_t e = -15; e < 21; e++) {
        printf("%+3d:", e);
        for(f_number_indx f = 0; f < F_NUMBER_MAX; f++){
            lux_to_EV_pair(e, f,
                           &speed,
                           &apt);
            printf(" ");
            print_f(apt);
            printf("-");
            print_speed(speed);
            printf(",");
        }
        printf("\n");
    }
    return 0;
}
