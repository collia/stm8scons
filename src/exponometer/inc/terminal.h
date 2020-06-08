#ifndef _TERMINAL_H_
#define _TERMINAL_H_
void term_print_char(fonts f, unsigned char ch, uint8_t column, uint8_t row);
void term_print(fonts f, unsigned char str[], uint8_t strlen, uint8_t column, uint8_t row);
void term_print_int(fonts f, int16_t val,  uint8_t column, uint8_t row, uint8_t len);
#endif
