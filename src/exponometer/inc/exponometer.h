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


typedef enum {
    ISO_4 = 7,
    ISO_5,
    ISO_6,
    ISO_8,
    ISO_10,
    ISO_12,
    ISO_16,
    ISO_20,
    ISO_25,
    ISO_32,
    ISO_MIN = ISO_32,
    ISO_40,
    ISO_50,
    ISO_64,
    ISO_80,
    ISO_100,
    ISO_BASE = ISO_100,
    ISO_125,
    ISO_160,
    ISO_200,
    ISO_250,
    ISO_320,
    ISO_400,
    ISO_500,
    ISO_640,
    ISO_800,
    ISO_1000,
    ISO_1250,
    ISO_1600,
    ISO_MAX = ISO_1600,
} iso_log_scale_t;

void ev_to_exp_pair(int16_t ev,
                    f_number_indx index,
                    uint16_t* speed,
                    uint8_t* aperture);

int16_t lux_to_EV(uint16_t lux, iso_log_scale_t iso);
uint16_t iso_from_log_to_arith(iso_log_scale_t iso);

#endif
