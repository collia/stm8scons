#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include "font.h"
#include "terminal.h"


void ssd1306_display_char(fonts f, unsigned char ch, uint8_t x, uint8_t y) {
    //printf("ssd1306_display_char");
    if(f == BIG_FONT) {
        printf("1306: B, %s x:%"PRIu8", y:%"PRIu8"\n",
               big_font_char_table[ch], x, y);
    } else if(f == SMALL_FONT) {
        printf("1306: s, %s x:%"PRIu8", y:%"PRIu8"\n",
               small_font_char_table[ch], x, y);
    }
}

void test_terminal_string() {
    
    const unsigned char iso_big_line[] = {
        BIG_FONT_LATIN_SMALL_LETTER_I,
        BIG_FONT_LATIN_SMALL_LETTER_S,
        BIG_FONT_LATIN_SMALL_LETTER_O,
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

    printf("iso 200\n");
    term_print(SMALL_FONT, iso_line, sizeof(iso_line), 0, 0);
    printf("ISO\n");
    term_print(BIG_FONT, iso_big_line, sizeof(iso_big_line), 0, 1);
}

void test_terminal_int() {
    const unsigned char minutes_line[] = {
        BIG_FONT_LATIN_SMALL_LETTER_M,
    };

    uint8_t rc = 0;
    printf("8000\n");
    rc = term_print_uint(BIG_FONT, 8000, 2, 2);
    printf("rc = %"PRIu8"\n", rc);
    printf("15\n");
    rc = term_print_uint(BIG_FONT, 15, 2, 2);
    printf("rc = %"PRIu8"\n", rc);
    printf("7\n");
    rc = term_print_uint(BIG_FONT, 7, 2, 2);
    printf("rc = %"PRIu8"\n", rc);
    printf("0\n");
    rc = term_print_uint(BIG_FONT, 0, 2, 2);
    printf("rc = %"PRIu8"\n", rc);
    printf("s-150\n");
    rc = term_print_uint(SMALL_FONT, -150, 2, 2);
    printf("rc = %"PRIu8"\n", rc);
    printf("1500m\n");
    rc = term_print_uint(BIG_FONT, 1500, 2, 2);
    term_print(BIG_FONT, minutes_line, sizeof(minutes_line), 2+rc, 1);
}

void test_terminal_fixed_point() {
    const unsigned char minutes_line[] = {
        BIG_FONT_LATIN_SMALL_LETTER_M,
    };

    uint8_t rc = 0;
    printf("800.1\n");
    rc = term_print_fixed_point(BIG_FONT, 8001, 0, 2, true);
    printf("rc = %"PRIu8"\n", rc);
    printf("1.5\n");
    rc = term_print_fixed_point(BIG_FONT, 15, 2, 2, false);
    printf("rc = %"PRIu8"\n", rc);
    printf("s-15.0\n");
    rc = term_print_fixed_point(SMALL_FONT, -150, 2, 2, true);
    printf("rc = %"PRIu8"\n", rc);
    printf("-150.0m\n");
    rc = term_print_fixed_point(BIG_FONT, -1500, 0, 2, true);
    term_print(BIG_FONT, minutes_line, sizeof(minutes_line), 0+rc, 1);
}


int main(void) {
    test_terminal_string();
    test_terminal_int();
    test_terminal_fixed_point();
}
