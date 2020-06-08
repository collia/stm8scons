#include <stdio.h>
#include <stdint.h>
#include <math.h>


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


int main(void) {
    // your code goes here
    uint32_t i = 0;
    for(i = 0; i<= 255; i++){
        printf("%d: %hhu %f\n", i,  log2_my(i), log2(i));
    }
    return 0;
}