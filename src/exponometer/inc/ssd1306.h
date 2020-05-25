#ifndef _SSD1306_H_
#define  _SSD1306_H_

#include "stm8s.h"
#define SSD1306_BLACK               0    ///< Draw 'off' pixels
#define SSD1306_WHITE               1    ///< Draw 'on' pixels
#define SSD1306_INVERSE             2    ///< Invert pixels


void ssd1306_init();
void ssd1306_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void ssd1306_clearDisplayBuffer(void);
//void ssd1306_display(void);
void ssd1306_display_video_buffer(uint8_t x_offset, uint8_t y_offset);
void ssd1306_clear_display(void);
void ssd1306_display_char(unsigned char ch, uint8_t x, uint8_t y);
#endif
