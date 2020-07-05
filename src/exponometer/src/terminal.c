/**
 * @file    terminal.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Module for minimal terminal implementation on
 *          SSD1306 screen.
 *
 * Allowed printing chars, integers and signed unsigned fixed point values.
 * If char is out of screen it will be skipped
 */

#include "stm8s.h"
#include "ssd1306.h"
#include "font.h"

#if defined SSD1306_128_64
#define TERMINAL_BIG_CHARS_COLUMNS (128/BIG_FONT_WIDTH)
#define TERMINAL_BIG_CHARS_ROWS    (64/BIG_FONT_HEIGHT)
#define TERMINAL_SMALL_CHARS_COLUMNS (128/SMALL_FONT_WIDTH)
#define TERMINAL_SMALL_CHARS_ROWS    (64/SMALL_FONT_HEIGHT)

#else
#error "Wasn't defined screen"
#endif

/**
 * @brief         Print char for font
 * @details       function finds char inf font table
 *                and prints it on specified
 * @param[in]     f - font index
 * @param[in]     ch - char index in font table
 * @param[in]     column - left coordinate in char coordinates
 * @param[in]     row - up coordinate in char coordinates
 */
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
/**
 * @brief         print string
 * @details       array should be array of indexes in font array
 * @param[in]     f - index of font table
 * @param[in]     str - array with chars indexes
 * @param[in]     strlen - number of elements in array
 * @param[in]     column - left coordinate
 * @param[in]     row - up coordinate
 */
void term_print(fonts f, const unsigned char str[], uint8_t strlen, uint8_t column, uint8_t row) {
    uint8_t i;
    for(i = 0; i < strlen; i++) {
            term_print_char(f, str[i], i+column, row);
    }
}
/**
 * @brief         print integer
 * @details       max len of converted string is 4 chars
 * @param[in]     f - font index
 * @param[in]     val - unsigned value
 * @param[in]     column - left coordinate
 * @param[in]     row - top coordinate
 *
 * @return        length of converted string
 */
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
/**
 * @brief         Print fixed point value
 * @details       print number, dot and last digit
 *                for example -123 will be -12.3
 *                            142 will be 14.2 or +14.2
 * @param[in]     f - font index
 * @param[in]     val - signed
 * @param[in]     column - left coordinate
 * @param[in]     row - top coordinate
 * @param[in]     sign - if true value will be printed with + or -
 *                       if false value will be printed as unsigned
 *
 * @return        length of converted value
 */
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
