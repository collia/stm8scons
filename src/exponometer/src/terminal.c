
#include "stm8s.h"
#include "ssd1306.h"

// This value should be synchronized with font/font.h

#define FONT_WIDTH 16
#define FONT_HEIGHT 16

#if defined SSD1306_128_64
#define TERMINAL_CHARS_COLUMNS (128/FONT_WIDTH)
#define TERMINAL_CHARS_ROWS    (64/FONT_HEIGHT)
#else
#error "Didn't defined screen"
#endif

void term_print_char(unsigned char ch, uint8_t column, uint8_t row) {
    if((column < TERMINAL_CHARS_COLUMNS) && (column < TERMINAL_CHARS_COLUMNS))
    {
        ssd1306_display_char(ch, column*FONT_WIDTH , row*FONT_HEIGHT);
    }
}

void term_print(unsigned char str[], uint8_t column, uint8_t row) {
    uint8_t i;
    for(i = column; i < TERMINAL_CHARS_COLUMNS; i++) {
        if(str[i-column] != 0) {
            term_print_char(str[i-column], i, row);
        }
        else
            break;
    }
}

void term_print_int(uint16_t val,  uint8_t column, uint8_t row, uint8_t len) {
    #define INT_STR_MAX_LEN 4
    char buffer[INT_STR_MAX_LEN+1];
    uint8_t i;
    buffer[len] = 0;
    for(i = 0; i < len; i++) {
        if(val) {
            buffer[len-1-i] = val % 10 + '0';
            val /= 10;
        } else {
            buffer[len-1-i] = '0';
        }
    }
    term_print(buffer, column, row);
}
