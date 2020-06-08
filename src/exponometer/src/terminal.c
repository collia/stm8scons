
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

void term_print(fonts f, unsigned char str[], uint8_t strlen, uint8_t column, uint8_t row) {
    uint8_t i;
    for(i = 0; i < strlen; i++) {
            term_print_char(f, str[i], i+column, row);
    }
}

void term_print_int(fonts f, int16_t val,  uint8_t column, uint8_t row, uint8_t len) {
    #define INT_STR_MAX_LEN 4
    unsigned char buffer[INT_STR_MAX_LEN+1];
    uint8_t i;
    uint8_t zero;
    uint8_t offset = 0;
    if(f == BIG_FONT) {
        zero = BIG_FONT_DIGIT_ZERO;
        buffer[0] = BIG_FONT_HYPHEN_MINUS;
    } else if (f == SMALL_FONT) {
        zero = SMALL_FONT_DIGIT_ZERO;
        buffer[0] = SMALL_FONT_HYPHEN_MINUS;
    }
    if(val < 0) {
        offset = 1;
        len--;
    }
    buffer[len] = 0;
    for(i = 0; i < len; i++) {
        if(val) {
            buffer[len-1-i + offset] = val % 10 + zero;
            val /= 10;
        } else {
            buffer[len-1-i + offset] = zero;
        }
    }
    term_print(f, buffer, len, column, row);
}
