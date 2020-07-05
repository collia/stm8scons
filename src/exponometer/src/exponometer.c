/**
 * @file    exponometer.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Exposure calculations
 *
 * In file is present routine for
 * - lux to EV calculation
 * - ISO compensation, defailt is ISO100
 * - converion EV and f index to f and speed
 */

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

static const uint16_t iso_arith_values[ISO_MAX-ISO_MIN+1] = {
    32,
    40,
    50,
    64,
    80,
    100,
    125,
    160,
    200,
    250,
    320,
    400,
    500,
    640,
    800,
    1000,
    1250,
    1600
};

static const uint8_t log2_table[0xff] = {
 0, //-inf
 0, //0.000000
 10, //1.000000
 16, //1.584963
 20, //2.000000
 23, //2.321928
 26, //2.584963
 28, //2.807355
 30, //3.000000
 32, //3.169925
 33, //3.321928
 35, //3.459432
 36, //3.584963
 37, //3.700440
 38, //3.807355
 39, //3.906891
 40, //4.000000
 41, //4.087463
 42, //4.169925
 42, //4.247928
 43, //4.321928
 44, //4.392317
 45, //4.459432
 45, //4.523562
 46, //4.584963
 46, //4.643856
 47, //4.700440
 48, //4.754888
 48, //4.807355
 49, //4.857981
 49, //4.906891
 50, //4.954196
 50, //5.000000
 50, //5.044394
 51, //5.087463
 51, //5.129283
 52, //5.169925
 52, //5.209453
 52, //5.247928
 53, //5.285402
 53, //5.321928
 54, //5.357552
 54, //5.392317
 54, //5.426265
 55, //5.459432
 55, //5.491853
 55, //5.523562
 56, //5.554589
 56, //5.584963
 56, //5.614710
 56, //5.643856
 57, //5.672425
 57, //5.700440
 57, //5.727920
 58, //5.754888
 58, //5.781360
 58, //5.807355
 58, //5.832890
 59, //5.857981
 59, //5.882643
 59, //5.906891
 59, //5.930737
 60, //5.954196
 60, //5.977280
 60, //6.000000
 60, //6.022368
 60, //6.044394
 61, //6.066089
 61, //6.087463
 61, //6.108524
 61, //6.129283
 61, //6.149747
 62, //6.169925
 62, //6.189825
 62, //6.209453
 62, //6.228819
 62, //6.247928
 63, //6.266787
 63, //6.285402
 63, //6.303781
 63, //6.321928
 63, //6.339850
 64, //6.357552
 64, //6.375039
 64, //6.392317
 64, //6.409391
 64, //6.426265
 64, //6.442943
 65, //6.459432
 65, //6.475733
 65, //6.491853
 65, //6.507795
 65, //6.523562
 65, //6.539159
 66, //6.554589
 66, //6.569856
 66, //6.584963
 66, //6.599913
 66, //6.614710
 66, //6.629357
 66, //6.643856
 67, //6.658211
 67, //6.672425
 67, //6.686501
 67, //6.700440
 67, //6.714246
 67, //6.727920
 67, //6.741467
 68, //6.754888
 68, //6.768184
 68, //6.781360
 68, //6.794416
 68, //6.807355
 68, //6.820179
 68, //6.832890
 68, //6.845490
 69, //6.857981
 69, //6.870365
 69, //6.882643
 69, //6.894818
 69, //6.906891
 69, //6.918863
 69, //6.930737
 69, //6.942515
 70, //6.954196
 70, //6.965784
 70, //6.977280
 70, //6.988685
 70, //7.000000
 70, //7.011227
 70, //7.022368
 70, //7.033423
 70, //7.044394
 71, //7.055282
 71, //7.066089
 71, //7.076816
 71, //7.087463
 71, //7.098032
 71, //7.108524
 71, //7.118941
 71, //7.129283
 71, //7.139551
 71, //7.149747
 72, //7.159871
 72, //7.169925
 72, //7.179909
 72, //7.189825
 72, //7.199672
 72, //7.209453
 72, //7.219169
 72, //7.228819
 72, //7.238405
 72, //7.247928
 73, //7.257388
 73, //7.266787
 73, //7.276124
 73, //7.285402
 73, //7.294621
 73, //7.303781
 73, //7.312883
 73, //7.321928
 73, //7.330917
 73, //7.339850
 73, //7.348728
 74, //7.357552
 74, //7.366322
 74, //7.375039
 74, //7.383704
 74, //7.392317
 74, //7.400879
 74, //7.409391
 74, //7.417853
 74, //7.426265
 74, //7.434628
 74, //7.442943
 75, //7.451211
 75, //7.459432
 75, //7.467606
 75, //7.475733
 75, //7.483816
 75, //7.491853
 75, //7.499846
 75, //7.507795
 75, //7.515700
 75, //7.523562
 75, //7.531381
 75, //7.539159
 75, //7.546894
 76, //7.554589
 76, //7.562242
 76, //7.569856
 76, //7.577429
 76, //7.584963
 76, //7.592457
 76, //7.599913
 76, //7.607330
 76, //7.614710
 76, //7.622052
 76, //7.629357
 76, //7.636625
 76, //7.643856
 77, //7.651052
 77, //7.658211
 77, //7.665336
 77, //7.672425
 77, //7.679480
 77, //7.686501
 77, //7.693487
 77, //7.700440
 77, //7.707359
 77, //7.714246
 77, //7.721099
 77, //7.727920
 77, //7.734710
 77, //7.741467
 77, //7.748193
 78, //7.754888
 78, //7.761551
 78, //7.768184
 78, //7.774787
 78, //7.781360
 78, //7.787903
 78, //7.794416
 78, //7.800900
 78, //7.807355
 78, //7.813781
 78, //7.820179
 78, //7.826548
 78, //7.832890
 78, //7.839204
 78, //7.845490
 79, //7.851749
 79, //7.857981
 79, //7.864186
 79, //7.870365
 79, //7.876517
 79, //7.882643
 79, //7.888743
 79, //7.894818
 79, //7.900867
 79, //7.906891
 79, //7.912889
 79, //7.918863
 79, //7.924813
 79, //7.930737
 79, //7.936638
 79, //7.942515
 79, //7.948367
 80, //7.954196
 80, //7.960002
 80, //7.965784
 80, //7.971544
 80, //7.977280
 80, //7.982994
 80, //7.988685
};
static const int8_t ev_iso_compensation_table[ISO_MAX-ISO_MIN+1] = {
 -16, //-1.643856, iso:32
 -13, //-1.321928, iso:40
 -10, //-1.000000, iso:50
 -6, //-0.643856, iso:64
 -3, //-0.321928, iso:80
 0, //0.000000, iso:100
 3, //0.321928, iso:125
 7, //0.678072, iso:160
 10, //1.000000, iso:200
 13, //1.321928, iso:250
 17, //1.678072, iso:320
 20, //2.000000, iso:400
 23, //2.321928, iso:500
 27, //2.678072, iso:640
 30, //3.000000, iso:800
 33, //3.321928, iso:1000
 36, //3.643856, iso:1250
 40, //4.000000, iso:1600
};


static uint16_t round_speed(uint16_t value);

/**
 * @brief         Function converts sensor Lux to exposure number for
 *                passed ISO value
 * @details       Function uses next idea:
 * From wikipedia:
 * https://en.wikipedia.org/wiki/Exposure_value#EV_as_a_measure_of_luminance_and_illuminance
 * And https://stackoverflow.com/questions/5401738/how-to-convert-between-lux-and-exposure-value
 * Lux = (2^EV) *2.5
 * from light sensor datasheet:
 * https://datasheets.maximintegrated.com/en/ds/MAX44009.pdf
 * lux = ((2^exp)*mantissa) *0.045
 *
 * So, than:
 * EV=log2(lux/2.5) = log2((2^exp)*mantissa*0.045/2.5) = exp + log2(mantissa) - 5.8
 *
 * @param[in]     lux - lux from sensor in format:
 *                bit 16   12    8         0
 *                      |    |exp |mantissa|
 * @param[in]     iso - iso in log scale format
 *
 * @return        return EV in fixed point format
 */
int16_t lux_to_EV(uint16_t lux, iso_log_scale_t iso) {
    int16_t ev;
    ev = ((lux >> 8) & 0xf) * 10;
    ev += log2_table[lux & 0xff];
    ev -= 58;
    if(iso <= ISO_MAX && iso >= ISO_MIN){
        //ev += ev_iso_compensation_table[iso-ISO_MIN];
    }
    return ev;
}
/**
 * @brief         Get speed and aperture from table for EV and index
 * @details       Function calculate speed and aperture for given
 *                EV and index for aperture value
 * @param[in]     ev - exposure with ISO compensation
 * @param[in]     index - number for current aperture
 * @param[out]    speed - shutter speed in format:
 *                if flag EXP_SPEED_SECONDS is set - value in seconds
 *                if EXP_SPEED_MINUTES - value in minutes
 *                otherwise value is second fraction
 *
 * @param[out]    param
 *
 */
void ev_to_exp_pair(int16_t ev,
                    f_number_indx index,
                    uint16_t* speed,
                    uint8_t* aperture) {
    uint8_t shift;
    if(index >=F_NUMBER_MAX) {
        *speed = 0;
        *aperture = 0;
    }
    if(ev % 10 >= 5) {
        ev = ev/10+1;
    } else {
        ev = ev/10;
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
/**
 * @brief         Convert iso to arith value for printing
 * @param[in]     iso in log scale
 *
 * @return        iso in arithmetic type
 */
uint16_t iso_from_log_to_arith(iso_log_scale_t iso) {
    if(iso <= ISO_MAX && iso >=ISO_MIN) {
        return iso_arith_values[iso-ISO_MIN];
    } else {
        return 0;
    }
}
/*
static uint8_t log2(uint8_t value) {
    uint8_t log2 = 0;
    while(value >> log2 != 0) {
        log2++;
    }
    log2 -= 1;
	log2 += (value >> (log2-1)) & 1 ;
    return log2;
}
*/

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
