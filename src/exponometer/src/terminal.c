
#include "stm8s.h"
#include "ssd1306.h"
#include "font.h"
// This value should be synchronized with font/font.h


#if defined SSD1306_128_64
#define TERMINAL_BIG_CHARS_COLUMNS (128/BIG_FONT_WIDTH)
#define TERMINAL_BIG_CHARS_ROWS    (64/BIG_FONT_HEIGHT)
#define TERMINAL_SMALL_CHARS_COLUMNS (128/SMALL_FONT_WIDTH)
#define TERMINAL_SMALL_CHARS_ROWS    (64/SMALL_FONT_HEIGHT)

#else
#error "Wasn't defined screen"
#endif

void term_print_char(fonts f, unsigned char ch, uint8_t column, uint8_t row) {
    switch(f) {
    case BIG_FONT:
        if((column < TERMINAL_BIG_CHARS_COLUMNS) && (column < TERMINAL_BIG_CHARS_COLUMNS))
        {
            ssd1306_display_char(f, ch, column*BIG_FONT_WIDTH , row*BIG_FONT_HEIGHT);
        }
        break;
    case SMALL_FONT:
        if((column < TERMINAL_SMALL_CHARS_COLUMNS) && (column < TERMINAL_SMALL_CHARS_COLUMNS))
        {
            ssd1306_display_char(f, ch, column*SMALL_FONT_WIDTH , row*SMALL_FONT_HEIGHT);
        }
    }
}

void term_print(fonts f, const unsigned char str[], uint8_t strlen, uint8_t column, uint8_t row) {
    uint8_t i;
    for(i = 0; i < strlen; i++) {
            term_print_char(f, str[i], i+column, row);
    }
}

uint8_t  term_print_uint(fonts f, uint16_t val,  uint8_t column, uint8_t row) {
    #define INT_STR_MAX_LEN 4
    unsigned char buffer[INT_STR_MAX_LEN+1];
    uint8_t i;
    uint8_t zero;
    if(f == BIG_FONT) {
        zero = BIG_FONT_DIGIT_ZERO;
    } else {
        zero = SMALL_FONT_DIGIT_ZERO;
    }
    if(val == 0){
        i=1;
        buffer[INT_STR_MAX_LEN-i+1] = zero;
    } else {
        for(i = 0; i <= INT_STR_MAX_LEN; i++) {
            //printf("[%d]=%d ",INT_STR_MAX_LEN-i, val%10);
            if(val) {
                buffer[INT_STR_MAX_LEN-i] = val % 10 + zero;
                val /= 10;
            } else {
                buffer[INT_STR_MAX_LEN-i] = zero;
                break;
            }
        }
    }

    //printf("\n i = %d\n", i);
    term_print(f, &buffer[INT_STR_MAX_LEN-i+1], i, column, row);
    return i;
}

uint8_t term_print_fixed_point(fonts f, int16_t val,  uint8_t column, uint8_t row, bool sign) {
    unsigned char dot;
    unsigned char zero;
    unsigned char plus;
    unsigned char minus;
    uint8_t rc = 0;
    if(f == BIG_FONT) {
        dot = BIG_FONT_FULL_STOP;
        zero = BIG_FONT_DIGIT_ZERO;
        plus = BIG_FONT_PLUS_SIGN;
        minus = BIG_FONT_HYPHEN_MINUS;
    } else {
        dot = SMALL_FONT_FULL_STOP;
        zero = SMALL_FONT_DIGIT_ZERO;
        plus = SMALL_FONT_PLUS_SIGN;
        minus = SMALL_FONT_HYPHEN_MINUS;
    }
    if(sign) {
        if(val < 0) {
            val = -val;
            term_print_char(f, minus,
                            column, row);
        } else {
            term_print_char(f, plus,
                            column, row);
        }
        rc++;
    }
    rc += term_print_uint(f, val/10, column+rc, row);
    term_print_char(f, dot,
                    column+rc, row);
    rc++;
    rc+=term_print_uint(f, val%10,
                    column+rc, row);
    return (rc);
}
