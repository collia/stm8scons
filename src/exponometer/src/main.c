/**
 * @file    main.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Expometer main routine
 *
 * File contains main function, general logic
 * for exponometer and interface routine
 *
 */

#include "stm8s.h"
#include "i2c.h"
#include "debug.h"
#include "ssd1306.h"
#include "terminal.h"
#include "max44009.h"
#include "buttons.h"
#include "time.h"
#include "message.h"
#include "exponometer.h"
#include "battery.h"
#include "stm8s_it.h"

#define DEVICE_SLEEP_TIMEOUT_MS  (20*1000)

static void clock_setup(void);
static void show_status();
static void continius_mode();

static void init_calibration(void);
static void init_exposure(void);
static void init_delay(void);
static void init_config(void);

static void run_calibration(uint8_t msg);
static void run_exposure(uint8_t msg);
static void run_delay(uint8_t msg);
static void run_config(uint8_t msg);

static void print_exposure(fonts font, int16_t ev, uint8_t f_idx, uint8_t column, uint8_t row);
static void print_exposure_error(fonts font, uint8_t column, uint8_t row);


static uint8_t current_f_idx = 0;
static int16_t min_ev = EV_INCORECT_VAL;
static int16_t max_ev = EV_INCORECT_VAL;
static iso_log_scale_t iso = ISO_BASE;
//Strings
static const unsigned char exp_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_E,
    BIG_FONT_LATIN_SMALL_LETTER_V,
    BIG_FONT_SPACE,
};

static const unsigned char lux_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_L,
    BIG_FONT_LATIN_SMALL_LETTER_U,
    BIG_FONT_LATIN_SMALL_LETTER_X,
};

static const unsigned char iso_big_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_I,
    BIG_FONT_LATIN_SMALL_LETTER_S,
    BIG_FONT_LATIN_SMALL_LETTER_O,
    BIG_FONT_SPACE,
};

static const unsigned char error_3_line[] = {
    BIG_FONT_HYPHEN_MINUS,
    BIG_FONT_HYPHEN_MINUS,
    BIG_FONT_HYPHEN_MINUS,
};

static const unsigned char error_2_line[] = {
    BIG_FONT_HYPHEN_MINUS,
    BIG_FONT_HYPHEN_MINUS,
};

static const unsigned char big_exponent_line[] = {
    BIG_FONT_SPACE,
    BIG_FONT_LATIN_SMALL_LETTER_E,
};

static const unsigned char ms_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_M,
    BIG_FONT_LATIN_SMALL_LETTER_S,
};

static const unsigned char empty_4_line[] = {
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
};

static const unsigned char empty_3_line[] = {
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
};

static const unsigned char iso_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_I,
    SMALL_FONT_LATIN_SMALL_LETTER_S,
    SMALL_FONT_LATIN_SMALL_LETTER_O,
    SMALL_FONT_SPACE,
};
static const unsigned char exposure_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_E,
    SMALL_FONT_LATIN_SMALL_LETTER_V,
    SMALL_FONT_SPACE,
};

static const unsigned char exp_fraction_line[] = {
    BIG_FONT_DIGIT_ONE,
    BIG_FONT_SOLIDUS,
};

static const unsigned char f_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_F,
};

static const unsigned char s_f_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_F,
};

static const unsigned char s_exp_fraction_line[] = {
    SMALL_FONT_DIGIT_ONE,
    SMALL_FONT_SOLIDUS,
};

static const unsigned char minutes_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_M,
};

static const unsigned char s_minutes_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_M,
};

static const unsigned char s_error_3_line[] = {
    SMALL_FONT_HYPHEN_MINUS,
    SMALL_FONT_HYPHEN_MINUS,
    SMALL_FONT_HYPHEN_MINUS,
};

static const unsigned char s_empty_3_line[] = {
    SMALL_FONT_SPACE,
    SMALL_FONT_SPACE,
    SMALL_FONT_SPACE,
};

/**
 * Enum with device work mode
 */
typedef enum {
    MODE_CALIBRATION,
#ifdef FEATURES_EXPONOMETER
    MODE_MEASHURE_EXPOSURE,
    MODE_CONFIG,
#endif
#ifdef FEATURES_SPEED
    MODE_MEASHURE_DELAY,
#endif
    MODE_MAX,
} device_mode;

typedef void (*init_hadler)(void);
typedef void (*run_hadler)(uint8_t msg);

static const init_hadler init_handlers[MODE_MAX] = {
    init_calibration,
#ifdef FEATURES_EXPONOMETER
    init_exposure,
    init_config,
#endif
#ifdef FEATURES_SPEED
    init_delay
#endif
};
static const run_hadler run_handlers[MODE_MAX] = {
    run_calibration,
#ifdef FEATURES_EXPONOMETER
    run_exposure,
    run_config,
#endif
#ifdef FEATURES_SPEED
    run_delay
#endif
};

int main()
{
    device_mode mode = MODE_MEASHURE_EXPOSURE;
    uint8_t msg, prev_btn_msg = MESSAGE_BUTTONS_RELEASED;
    bool is_long_pressed;
    bool is_sleep = FALSE;
    // Periphery initialization
    debug_init();
    clock_setup();
    time_init();
    i2c_master_init();
    buttons_init();
    battery_init();

    enableInterrupts();
    // external devices driver initialization
    ssd1306_init();
    max44009_init();

    init_handlers[mode]();
    while(1) {
        show_status();

        msg = message_resv();
        if(msg == MESSAGE_BUTTONS_RELEASED) {
            if(message_get_msg_time(msg) -
               message_get_msg_time(prev_btn_msg) > 2000) {
                is_long_pressed = TRUE;
            } else {
                is_long_pressed = FALSE;
            }

            switch(prev_btn_msg) {
            case MESSAGE_MODE_BUTTON:
                if(is_long_pressed) {
                    if(++mode >= MODE_MAX) {
                        mode = 0;
                    }
                    init_handlers[mode]();
                }
                break;
            }
        } else if(msg == MESSAGE_MODE_BUTTON ||
                  msg == MESSAGE_UP_BUTTON ||
                  msg == MESSAGE_DOWN_BUTTON) {
            prev_btn_msg  = msg;
            ssd1306_display_on();
            is_sleep = FALSE;
            message_resend(MESSAGE_SLEEP_MODE, DEVICE_SLEEP_TIMEOUT_MS);
        } else if(msg == MESSAGE_BUTTONS_IRQ) {
            buttons_send_status();
            button_set_irq_en(1);
        } else if(msg == MESSAGE_SLEEP_MODE) {
            is_sleep = TRUE;
            ssd1306_display_off();
        }
        if(msg == MESSAGE_BUTTONS_RELEASED) {
            run_handlers[mode](prev_btn_msg);
            prev_btn_msg = MESSAGE_NUMBER;
        } else {
            run_handlers[mode](MESSAGE_NUMBER);
        }
    }
}

static void clock_setup(void) {
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
    CLK_ClockSwitchCmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV2);
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
                          DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
}

static void init_calibration(void) {
    term_print(SMALL_FONT, iso_line, sizeof(iso_line), 0, 0);
    term_print_uint(SMALL_FONT,
                    10,
                    1, 1);

    term_print_uint(SMALL_FONT,
                    iso_from_log_to_arith(iso),
                    4, 0);
    term_print(BIG_FONT, lux_line, sizeof(lux_line), 0, 1);
    term_print(BIG_FONT, error_3_line, sizeof(error_3_line), 0, 2);
    term_print(BIG_FONT, big_exponent_line, sizeof(big_exponent_line), 3, 2);
    term_print(BIG_FONT, error_2_line, sizeof(error_2_line), 5, 2);
    term_print(BIG_FONT, exp_line, sizeof(exp_line), 0, 3);
    term_print(BIG_FONT, error_2_line, sizeof(error_2_line), 4, 3);
}
#ifdef FEATURES_EXPONOMETER
static void init_exposure(void) {

    term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line), 0, 0);
    term_print(SMALL_FONT, iso_line, sizeof(iso_line), 0, 0);
    term_print_uint(SMALL_FONT,
                    iso_from_log_to_arith(iso),
                    4, 0);
    term_print(SMALL_FONT, exposure_line, sizeof(exposure_line), 0, 1);

    term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line), 4, 2);

    //print_exposure(BIG_FONT, -5, 4, 0, 1);
    print_exposure_error(BIG_FONT, 0, 1);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 0, 3);
    term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line), 4, 3);
    //print_exposure(SMALL_FONT,-1, 0, 6);
    print_exposure_error(SMALL_FONT, 0, 6);
    //print_exposure(SMALL_FONT,1, 1, 10, 6);
    print_exposure_error(SMALL_FONT, 10, 6);

}
static void init_config(void) {
    uint8_t rc;
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 0, 0);
    term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line), 4, 0);

    term_print(BIG_FONT, iso_big_line, sizeof(iso_big_line), 0, 1);

    rc = term_print_uint(BIG_FONT,
                    iso_from_log_to_arith(iso),
                    sizeof(iso_big_line), 1);

    term_print(BIG_FONT,
               empty_4_line,
               sizeof(empty_4_line),
               rc + sizeof(iso_big_line), 1);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 0, 2);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 4, 2);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 0, 3);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 4, 3);

}
#endif
#ifdef FEATURES_SPEED
static void init_delay(void) {
    term_print(BIG_FONT, error_3_line, sizeof(error_3_line), 0, 1);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 3, 1);
    term_print(BIG_FONT, ms_line, sizeof(ms_line), 3, 2);
    term_print(BIG_FONT, error_3_line, sizeof(error_3_line), 0, 2);
    term_print(BIG_FONT, empty_3_line, sizeof(error_3_line), 5, 2);
}
#endif

static void show_status(void) {
    static int i = 0;
    uint8_t voltage;
    if(i == 0) {
        if(battery_get_voltage(&voltage)){
            if(voltage > 30) {
                ssd1306_display_char(SPECIAL_FONT, SPECIAL_FONT_BAT_CHRG, 128 - 16, 0);
            } else if(voltage >= 29) {
                ssd1306_display_char(SPECIAL_FONT, SPECIAL_FONT_BAT_FULL, 128 - 16, 0);
            } else if(voltage >= 28) {
                ssd1306_display_char(SPECIAL_FONT, SPECIAL_FONT_BAT_HALF, 128 - 16, 0);
            } else {
                ssd1306_display_char(SPECIAL_FONT, SPECIAL_FONT_BAT_EMPTY, 128 - 16, 0);
            }
        }
    }
    if(++i > 16000)
        i = 0;
}

static void run_calibration(uint8_t msg) {
    static int i = 0;
    int lux=2;
    uint8_t rc;
    uint8_t voltage = 0;
    //for(i = 0; i < 24000; i++);
    //debug_blink_1_sec();
    if(i++ < 1600) {
        return;
    }
    i = 0;
    lux = max44009_get_lux();
    if(lux < 0) {
        term_print(BIG_FONT, error_3_line, sizeof(error_3_line), 0, 2);
        term_print(BIG_FONT, error_2_line, sizeof(error_2_line), 5, 2);
        term_print(BIG_FONT, error_2_line, sizeof(error_2_line), 4, 3);
    } else {
        rc = term_print_uint(BIG_FONT,
            lux & 0xff,
            0, 2);
        term_print(BIG_FONT,
                   big_exponent_line,
                   sizeof(big_exponent_line), rc, 2);
        rc += term_print_uint(BIG_FONT,
            (lux & 0xf00) >> 8,
            rc+sizeof(big_exponent_line), 2);
        term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line),
                   rc+sizeof(big_exponent_line), 2);

        rc = term_print_fixed_point(BIG_FONT, lux_to_EV(lux, iso), 3, 3, TRUE);
        //term_print_uint(BIG_FONT, lux_to_EV(lux, iso), 3, 3);
        term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line),
                   rc+3, 3);
    }
    if(battery_get_voltage(&voltage)) {
        term_print_fixed_point(SMALL_FONT,
                               voltage,
                               1, 1, TRUE);

    }
}
#ifdef FEATURES_EXPONOMETER
static void run_exposure(uint8_t msg) {
    bool update = FALSE;
    static int16_t ev=0;

    switch(msg) {
    case MESSAGE_MODE_BUTTON:
        ev  = lux_to_EV(max44009_get_lux(), iso);
        update = TRUE;
        break;
    case MESSAGE_UP_BUTTON:
        if(current_f_idx == F_NUMBER_MAX-1) {
            current_f_idx=0;
        } else {
            current_f_idx++;
        }
        update = TRUE;
        break;
    case MESSAGE_DOWN_BUTTON:
        if(current_f_idx == 0) {
            current_f_idx=F_NUMBER_MAX-1;
        } else {
            current_f_idx--;
        }
        update = TRUE;
        break;
    }
    if(update) {
        print_exposure(BIG_FONT, ev, current_f_idx, 0, 1);
        if((min_ev == EV_INCORECT_VAL) || min_ev >= ev) {
            min_ev = ev;
            print_exposure(SMALL_FONT,ev, current_f_idx, 0, 6);
        }
        if((max_ev == EV_INCORECT_VAL) || max_ev <= ev) {
            max_ev = ev;
            print_exposure(SMALL_FONT,ev, current_f_idx, 10, 6);
        }
    }
}
#endif
#ifdef FEATURES_SPEED
static void run_delay(uint8_t msg) {
}
#endif
#ifdef FEATURES_EXPONOMETER
static void run_config(uint8_t msg) {
    bool update = FALSE;
    static int16_t ev=0;
    uint8_t rc;

    switch(msg) {
    case MESSAGE_UP_BUTTON:
        if(iso == ISO_MAX) {
            iso=ISO_MIN;
        } else {
            iso++;
        }
        update = TRUE;
        break;
    case MESSAGE_DOWN_BUTTON:
        if(iso == ISO_MIN) {
            iso=ISO_MAX;
        } else {
            iso--;
        }
        update = TRUE;
        break;
    }
    if(update) {
            rc = term_print_uint(BIG_FONT,
                    iso_from_log_to_arith(iso),
                    sizeof(iso_big_line), 1);

            term_print(BIG_FONT,
                       empty_4_line,
                       sizeof(empty_4_line),
                       rc + sizeof(iso_big_line), 1);

    }

}
#endif

static void print_exposure(fonts font, int16_t ev, uint8_t f_idx, uint8_t column, uint8_t row) {
    const unsigned char *apt_line;
    const unsigned char *e_line;
    const unsigned char *e_minutes_line;
    const unsigned char *spaces;
    //unsigned char dot;
    //unsigned char zero;
    uint16_t speed;
    uint8_t apt;
    uint8_t rc;

    if(font == BIG_FONT) {
        e_line = exp_fraction_line;
        apt_line = f_line;
        e_minutes_line = minutes_line;
        spaces = empty_3_line;
    } else {
        e_line = s_exp_fraction_line;
        apt_line = s_f_line;
        e_minutes_line = s_minutes_line;
        spaces = s_empty_3_line;
    }
    term_print_fixed_point(SMALL_FONT,
                           ev,
                           3,1,TRUE);
    ev_to_exp_pair(ev, f_idx,
                   &speed,
                   &apt);
    if((speed & (~(EXP_SPEED_MINUTES|EXP_SPEED_SECONDS))) == 0) {
        print_exposure_error(font, column, row);
    } else {
        if(speed & EXP_SPEED_MINUTES) {
            rc = term_print_uint(font,
                       (speed & (~EXP_SPEED_MINUTES)),
                       column, row);
            term_print(font, e_minutes_line, sizeof(minutes_line),
                       column+rc, row);
            term_print(font, spaces, sizeof(empty_3_line),
                       column+rc+sizeof(minutes_line), row);

        } else if (speed & EXP_SPEED_SECONDS) {
            rc = term_print_uint(font,
                           (speed & (~EXP_SPEED_SECONDS)),
                           column, row);
            term_print(font, spaces, sizeof(empty_3_line), column+rc, row);
            term_print(font, spaces, sizeof(empty_3_line), column+rc+3, row);
        } else {
            term_print(font, e_line, sizeof(exp_fraction_line), column, row);
            rc = term_print_uint(font,
                                 speed,
                                 column+2, row);
            term_print(font, spaces, sizeof(empty_3_line), column+2+rc, row);
        }


        term_print(font, apt_line, sizeof(f_line), column, row+1);
        if(apt & F_NUM_POINT) {
            term_print_fixed_point(font, apt & (~F_NUM_POINT),
                                   column+1, row+1,FALSE );
            term_print(font, spaces, sizeof(empty_3_line), column+1+3, row+1);

        } else {
            rc = term_print_uint(font,
                           apt & (~F_NUM_POINT),
                           column+1, row+1);
            term_print(font, spaces, sizeof(empty_3_line), column+1+rc, row+1);
        }
    }
}

static void print_exposure_error(fonts font, uint8_t column, uint8_t row) {
    const unsigned char *apt_line;
    const unsigned char *e_line;
    const unsigned char *error;

    if(font == BIG_FONT) {
        e_line = exp_fraction_line;
        apt_line = f_line;
        error = error_3_line;
    } else {
        e_line = s_exp_fraction_line;
        apt_line = s_f_line;
        error = s_error_3_line;
    }

    term_print(font, error, sizeof(error_3_line),
               column, row);
    term_print(font, error, sizeof(error_3_line),
               column+sizeof(error_3_line), row);

    term_print(font, apt_line, sizeof(f_line), column, row+1);
    term_print(font, error, sizeof(error_3_line),
               column+sizeof(f_line), row+1);
}


/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
    debug_blink_slow_forever();
    while(1);
}
