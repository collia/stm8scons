#ifndef _EXPONOMENTER_H_
#define _EXPONOMENTER_H_

typedef enum {
    F_NUMBER_1_0,
    F_NUMBER_1_4,
    F_NUMBER_2_0,
    F_NUMBER_2_8,
    F_NUMBER_4_0,
    F_NUMBER_5_6,
    F_NUMBER_8_0,
    F_NUMBER_11,
    F_NUMBER_16,
    F_NUMBER_22,
    F_NUMBER_32,
    F_NUMBER_45,
    F_NUMBER_64,
    F_NUMBER_MAX
} f_number_indx;

#define EV_INCORECT_VAL    (127)

#define F_NUM_POINT        (1<<7)

#define EXP_SPEED_SECONDS  (1<<14)
#define EXP_SPEED_MINUTES  (1<<15)

void lux_to_EV_pair(int8_t ev, f_number_indx index,
                    uint16_t* speed,
                    uint8_t* aperture);

int8_t lux_to_EV(uint16_t lux);

#endif
