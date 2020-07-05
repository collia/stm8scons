#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "exponometer.h"

static void print_f(uint8_t f) {
    if(f & F_NUM_POINT) {
        printf("f%2.1f", ((float)(f & (~F_NUM_POINT)))/10);
    } else {
        printf("f%2"PRIu8, f);
    }
}


static void print_speed(uint16_t s) {
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

static void print_iso() {
    for(int i = ISO_MIN; i <= ISO_MAX; i++) {
        printf("%d = %4"PRIu16"\n",i , iso_from_log_to_arith(i)); 
    }
}

static void print_ev_iso_compensation() {
    int8_t value;
    uint16_t iso;
    double real_value;
    printf("static const int8_t ev_iso_compensation_table[ISO_MAX-ISO_MIN+1] = {\n");
    for(uint8_t i = ISO_MIN; i <= ISO_MAX; i++) {
        iso = iso_from_log_to_arith(i);
        real_value = log2((double)iso/100);
        value = (int8_t)(lround(real_value*10));
        printf(" %"PRIi8", //%f, iso:%"PRIu16"\n", value, real_value, iso);
    }
    printf("};\n");
}

int main(void) {
    uint16_t speed;
    uint8_t apt;
    for(int16_t e = -150; e < 210; e++) {
        printf("%+3d:", e);
        for(f_number_indx f = 0; f < F_NUMBER_MAX; f++){
            ev_to_exp_pair(e, f,
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
    print_iso();
    print_ev_iso_compensation();
    return 0;
}
