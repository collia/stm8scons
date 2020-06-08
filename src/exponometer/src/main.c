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
#include "stm8s_it.h"


static void clock_setup(void);
static void show_status();
static void continius_mode();

static void init_calibration(void);
static void init_exposure(void);
static void init_delay(void);

static void run_calibration(uint8_t msg);
static void run_exposure(uint8_t msg);
static void run_delay(uint8_t msg);

static void print_exposure(fonts font, int8_t ev, uint8_t f_idx, uint8_t column, uint8_t row);
static void print_exposure_error(fonts font, uint8_t column, uint8_t row);


static uint8_t current_f_idx = 0;
static int8_t min_ev = EV_INCORECT_VAL;
static int8_t max_ev = EV_INCORECT_VAL;
//Strings
const unsigned char exp_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_E,
    BIG_FONT_LATIN_SMALL_LETTER_V,
    BIG_FONT_SPACE,
};

const unsigned char lux_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_L,
    BIG_FONT_LATIN_SMALL_LETTER_U,
    BIG_FONT_LATIN_SMALL_LETTER_X,
};


const unsigned char iso_big_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_I,
    BIG_FONT_LATIN_SMALL_LETTER_S,
    BIG_FONT_LATIN_SMALL_LETTER_O,
};

const unsigned char error_3_line[] = {
    BIG_FONT_HYPHEN_MINUS,
    BIG_FONT_HYPHEN_MINUS,
    BIG_FONT_HYPHEN_MINUS,
};
const unsigned char error_2_line[] = {
    BIG_FONT_HYPHEN_MINUS,
    BIG_FONT_HYPHEN_MINUS,
};
const unsigned char big_exponent_line[] = {
    BIG_FONT_SPACE,
    BIG_FONT_LATIN_SMALL_LETTER_E,
};
const unsigned char ms_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_M,
    BIG_FONT_LATIN_SMALL_LETTER_S,
};

const unsigned char empty_4_line[] = {
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
};
const unsigned char empty_3_line[] = {
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
    BIG_FONT_SPACE,
};

const unsigned char iso_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_I,
    SMALL_FONT_LATIN_SMALL_LETTER_S,
    SMALL_FONT_LATIN_SMALL_LETTER_O,
    SMALL_FONT_SPACE,
    SMALL_FONT_DIGIT_TWO,
    SMALL_FONT_DIGIT_ZERO,
    SMALL_FONT_DIGIT_ZERO,
};
const unsigned char exp_correct_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_E,
    SMALL_FONT_LATIN_SMALL_LETTER_X,
    SMALL_FONT_LATIN_SMALL_LETTER_P,
    SMALL_FONT_SPACE,
    SMALL_FONT_DIGIT_ZERO,
};

const unsigned char exp_fraction_line[] = {
    BIG_FONT_DIGIT_ONE,
    BIG_FONT_SOLIDUS,
};

const unsigned char f_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_F,
};

const unsigned char s_f_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_F,
};

const unsigned char s_exp_fraction_line[] = {
    SMALL_FONT_DIGIT_ONE,
    SMALL_FONT_SOLIDUS,
};

const unsigned char minutes_line[] = {
    BIG_FONT_LATIN_SMALL_LETTER_M,
};

const unsigned char s_minutes_line[] = {
    SMALL_FONT_LATIN_SMALL_LETTER_M,
};

const unsigned char s_error_3_line[] = {
    SMALL_FONT_HYPHEN_MINUS,
    SMALL_FONT_HYPHEN_MINUS,
    SMALL_FONT_HYPHEN_MINUS,
};

const unsigned char s_empty_3_line[] = {
    SMALL_FONT_SPACE,
    SMALL_FONT_SPACE,
    SMALL_FONT_SPACE,
};
/*
const unsigned char dot_line[] = {
    BIG_FONT_FULL_STOP,
};
const unsigned char s_dot_line[] = {
    SMALL_FONT_FULL_STOP,
};
*/
typedef enum {
    MODE_CALIBRATION,
    MODE_MEASHURE_EXPOSURE,
    MODE_CONFIG,
    MODE_MEASHURE_DELAY,
    MODE_MAX,
} device_mode;

typedef void (*init_hadler)(void);
typedef void (*run_hadler)(uint8_t msg);

static const init_hadler init_handlers[MODE_MAX] = {
    init_calibration,
    init_exposure,
    init_config,
    init_delay
};
static const run_hadler run_handlers[MODE_MAX] = {
    run_calibration,
    run_exposure,
    run_config,
    run_delay
};

int main()
{
    device_mode mode = MODE_MEASHURE_EXPOSURE;
    uint8_t msg, prev_btn_msg = MESSAGE_BUTTONS_RELEASED;
    bool is_long_pressed;

    debug_init();
    clock_setup();
    time_init();
    i2c_master_init();
    buttons_init();

    enableInterrupts();

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
                /*
            case MESSAGE_UP_BUTTON:
                //term_print("US", 4, 0);
                break;
            case MESSAGE_DOWN_BUTTON:
                //term_print("DS", 4, 0);
                break;
                */
            }
        } else if(msg == MESSAGE_MODE_BUTTON ||
                  msg == MESSAGE_UP_BUTTON ||
                  msg == MESSAGE_DOWN_BUTTON) {
            prev_btn_msg  = msg;
            ssd1306_display_on();
        } else if(msg == MESSAGE_BUTTONS_IRQ) {
            buttons_send_status();
            button_set_irq_en(1);
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
    //CLK_DeInit();

    //CLK_HSECmd(DISABLE);
    //CLK_LSICmd(DISABLE);
    //CLK_HSICmd(ENABLE);
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
    CLK_ClockSwitchCmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV2);
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
                          DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
    //CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
    //CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
    //CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
    //CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
    //CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
}

static void init_calibration(void) {
    //term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 0, 1);
    term_print(BIG_FONT, lux_line, sizeof(lux_line), 0, 1);
    term_print(BIG_FONT, error_3_line, sizeof(error_3_line), 0, 2);
    term_print(BIG_FONT, big_exponent_line, sizeof(big_exponent_line), 3, 2);
    term_print(BIG_FONT, error_2_line, sizeof(error_2_line), 5, 2);
    term_print(BIG_FONT, exp_line, sizeof(exp_line), 0, 3);
    term_print(BIG_FONT, error_2_line, sizeof(error_2_line), 4, 3);
}
static void init_exposure(void) {

    term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line), 0, 0);
    term_print(SMALL_FONT, iso_line, sizeof(iso_line), 0, 0);
    term_print(SMALL_FONT, exp_correct_line, sizeof(exp_correct_line), 0, 1);

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
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 0, 0);
    term_print(BIG_FONT, empty_3_line, sizeof(empty_3_line), 4, 0);

    term_print(BIG_FONT, iso_big_line, sizeof(iso_big_line), 0, 1);
    term_print(BIG_FONT, exp_line, sizeof(exp_line), 0, 2);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 0, 3);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 4, 3);

}

static void init_delay(void) {
    term_print(BIG_FONT, error_3_line, sizeof(error_3_line), 0, 1);
    term_print(BIG_FONT, empty_4_line, sizeof(empty_4_line), 3, 1);
    term_print(BIG_FONT, ms_line, sizeof(ms_line), 3, 2);
    term_print(BIG_FONT, error_3_line, sizeof(error_3_line), 0, 2);
    term_print(BIG_FONT, empty_3_line, sizeof(error_3_line), 5, 2);
}


static void show_status(void) {
    static int i = 0;
    if(i == 0) {
        ssd1306_display_char(SPECIAL_FONT, SPECIAL_FONT_BAT_CHRG, 128 - 16, 0);
    }
    if(++i > 16000)
        i = 0;
}

static void run_calibration(uint8_t msg) {
    static int i = 0;
    int lux=2;
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
        term_print_int(BIG_FONT,
            lux & 0xff,
            0, 2, 3);
        term_print_int(BIG_FONT,
            (lux & 0xf00) >> 8,
            5, 2, 2);
        term_print_int(BIG_FONT, lux_to_EV(lux), 4, 3, 2);
    }
}
static void run_exposure(uint8_t msg) {
    bool update = FALSE;
    static int8_t ev=0;

    switch(msg) {
    case MESSAGE_MODE_BUTTON:
        ev  = lux_to_EV(max44009_get_lux());
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
static void run_delay(uint8_t msg) {
}
static void run_config(uint8_t msg) {
}


static void print_exposure(fonts font, int8_t ev, uint8_t f_idx, uint8_t column, uint8_t row) {
    const unsigned char *apt_line;
    const unsigned char *e_line;
    const unsigned char *e_minutes_line;
    const unsigned char *spaces;
    unsigned char dot;
    unsigned char zero;
    uint16_t speed;
    uint8_t apt;

    if(font == BIG_FONT) {
        e_line = exp_fraction_line;
        apt_line = f_line;
        e_minutes_line = minutes_line;
        spaces = empty_3_line;
        dot = BIG_FONT_FULL_STOP;
        zero = BIG_FONT_DIGIT_ZERO;
    } else {
        e_line = s_exp_fraction_line;
        apt_line = s_f_line;
        e_minutes_line = s_minutes_line;
        spaces = s_empty_3_line;
        dot = SMALL_FONT_FULL_STOP;
        zero = SMALL_FONT_DIGIT_ZERO;
    }
    lux_to_EV_pair(ev, f_idx,
                   &speed,
                   &apt);
    if((speed & (~(EXP_SPEED_MINUTES|EXP_SPEED_SECONDS))) == 0) {
        print_exposure_error(font, column, row);
    } else {
        if(speed & EXP_SPEED_MINUTES) {
            term_print_int(font,
                       (speed & (~EXP_SPEED_MINUTES)),
                       column, row, 4);
            term_print(font, e_minutes_line, sizeof(minutes_line),
                       column+4, row);
            term_print(font, spaces, sizeof(empty_3_line),
                       column+5, row);

        } else if (speed & EXP_SPEED_SECONDS) {
            term_print_int(font,
                           (speed & (~EXP_SPEED_SECONDS)),
                           column, row, 2);
            term_print(font, spaces, sizeof(empty_3_line), column+2, row);
            term_print(font, spaces, sizeof(empty_3_line), column+2+3, row);
        } else {
            term_print(font, e_line, sizeof(exp_fraction_line), column, row);
            term_print_int(font,
                       speed,
                       column+2, row, 4);
        }


        term_print(font, apt_line, sizeof(f_line), column, row+1);
        if(apt & F_NUM_POINT) {
            //term_print_int(font,
            //               (apt & (~F_NUM_POINT))/10,
            //               column+1, row+1, 1);
            //term_print(font, dot, sizeof(dot_line), column+1+1, row+1);
            //term_print_int(font,
            //               (apt & (~F_NUM_POINT))%10,
            //               column+3, row+1, 1);
            term_print_char(font, (apt & (~F_NUM_POINT))/10 + zero,
                            column+1, row+1);
            term_print_char(font, dot,
                            column+2, row+1);
            term_print_char(font, (apt & (~F_NUM_POINT))%10 + zero,
                            column+3, row+1);
            term_print(font, spaces, sizeof(empty_3_line), column+1+3, row+1);
        } else {
            term_print_int(font,
                           apt & (~F_NUM_POINT),
                           column+1, row+1, 2);
            term_print(font, spaces, sizeof(empty_3_line), column+1+2, row+1);
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
}
