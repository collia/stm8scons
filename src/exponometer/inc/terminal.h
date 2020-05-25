#ifndef _TERMINAL_H_
#define _TERMINAL_H_
void term_print_char(unsigned char ch, uint8_t column, uint8_t row);
void term_print(unsigned char str[], uint8_t column, uint8_t row);
void term_print_int(uint16_t val,  uint8_t column, uint8_t row, uint8_t len);
#endif
