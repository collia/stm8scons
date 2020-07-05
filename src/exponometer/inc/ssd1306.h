/**
 * @file    ssd1306.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   SSD1306 screen driver
 *
 * Now is supported only 128x64 screen
 */

#ifndef _SSD1306_H_
#define  _SSD1306_H_

#include "stm8s.h"
#include "font.h"
#define SSD1306_BLACK               0    ///< Draw 'off' pixels
#define SSD1306_WHITE               1    ///< Draw 'on' pixels
#define SSD1306_INVERSE             2    ///< Invert pixels


void ssd1306_init();
void ssd1306_display_on();
void ssd1306_display_off();
#ifdef SSD1306_GRAPH_MODE
void ssd1306_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void ssd1306_clearDisplayBuffer(void);
void ssd1306_display_video_buffer(uint8_t x_offset, uint8_t y_offset);
#endif
void ssd1306_clear_display(void);
void ssd1306_display_char(fonts f, unsigned char ch, uint8_t x, uint8_t y);


#endif
