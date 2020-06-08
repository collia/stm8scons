#include "stm8s.h"
#include "exponometer.h"

static const uint8_t f_values[F_NUMBER_MAX] = {
    10 | F_NUM_POINT,
    14 | F_NUM_POINT,
    20 | F_NUM_POINT,
    28 | F_NUM_POINT,
    40 | F_NUM_POINT,
    56 | F_NUM_POINT,
    80 | F_NUM_POINT,
    11,
    16,
    22,
    32,
    45,
    64
};

/*
 From wikipedia: https://en.wikipedia.org/wiki/Exposure_value#EV_as_a_measure_of_luminance_and_illuminance
And https://stackoverflow.com/questions/5401738/how-to-convert-between-lux-and-exposure-value
Lux = (2^EV) *2.5
and from light sensor datasheet: https://datasheets.maximintegrated.com/en/ds/MAX44009.pdf

lux = ((2^exp)*mantissa) *0.045

So, than:
EV=log2(lux/2.5) = log2((2^exp)*mantissa*0.045/2.5) = exp + log2(mantissa) - 5.8

And 
*/

static uint8_t log2(uint8_t value);
static uint16_t round_speed(uint16_t value);

int8_t lux_to_EV(uint16_t lux) {
    int8_t ev;
    ev = (lux >> 8) & 0xf;
    ev += log2(lux & 0xff);
    ev -= 6;
    return ev;
}

void lux_to_EV_pair(int8_t ev, f_number_indx index,
                    uint16_t* speed,
                    uint8_t* aperture) {
    uint8_t shift;
    if(index >=F_NUMBER_MAX) {
        *speed = 0;
        *aperture = 0;
    }
    
    *aperture = f_values[index];
    *speed = 0;
    if( ev > (int8_t)index) {
        shift = (uint8_t)(ev - index);
    } else {
        if(ev < 0) {
            shift = (uint8_t)(-ev+index);
        } else {
            shift = (uint8_t)(index - ev);
        }
        if(shift > 6) {
            *speed |= EXP_SPEED_MINUTES;
            shift -= 6;
        } else {
            *speed |= EXP_SPEED_SECONDS;
        }
    }

    if(shift < 14) {
        *speed |= round_speed(1 << shift);
    }
}
static uint8_t log2(uint8_t value) {
    uint8_t log2 = 0;
    while(value >> log2 != 0) {
        log2++;
    }
    log2 -= 1;
	log2 += (value >> (log2-1)) & 1 ;
    return log2;
}


static uint16_t round_speed(uint16_t value) {
    if(value > 1000) {
        return (value/1000)*1000;
    } else if(value > 500) {
        return (value/100)*100;
    } else if(value > 200) {
        return (value/50)*50;
    } else if(value > 100) {
        return (value/25)*25;
    } else if(value > 20) {
        return (value/10)*10;
    } else if(value > 10) {
        return (value/5)*5;
    } else {
        return value;
    }
}
