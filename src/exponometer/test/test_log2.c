#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>


static uint8_t log2_my(uint8_t value) {
    uint8_t i, log2 = 1;
    while((value >> log2) != 0) {
        printf("%d ", value >> log2);
        log2++;
    }
//    for(i = 8; i > 0; i >>= 1){
//        if((1 << (i+log2)) <= value) {
//            log2+=i;
//        }
//    }
    log2 -= 1;
    log2 += (value >> (log2-1)) & 1 ;
    return log2;
}

static void generate_log2_array() {
    uint8_t log_value;
    double real_log_value;
    printf("static const uint8_t log2_table[0xff] = {\n");
    for (int i = 0; i < 0xff; i++) {
        real_log_value = log2(i);
        log_value = (uint8_t)(0xff & lround(real_log_value*10));
        printf(" %"PRIu8", //%f\n", log_value, real_log_value);
    }
    printf("};\n");
}


int main(void) {
    // your code goes here
    uint32_t i = 0;
    for(i = 0; i<= 255; i++){
        printf("%d: %hhu %f\n", i,  log2_my(i), log2(i));
    }
    generate_log2_array();
    return 0;
}
