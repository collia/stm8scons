/**
 * @file    ssd1306.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   SSD1306 screen driver
 *
 * Now is supported only 128x64 screen
 */

#include <stdlib.h>
#include <string.h>

#include "ssd1306.h"
#include "stm8s.h"
#include "i2c.h"

#include "font.h"
// Configuration

#define SSD1306_I2C_ADDR   (0x78)


#define ssd1306_swap(a, b) \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

#define SSD1306_SCREEN_ROTATION  (0)

//Registers
#define SSD1306_SHIFT_DC           0x06
#define SSD1306_SHIFT_CO           0x07
#define SSD1306_COMMAND            (0<<SSD1306_SHIFT_CO) | (0<<SSD1306_SHIFT_DC)
#define SSD1306_DATA               (0<<SSD1306_SHIFT_CO) | (1<<SSD1306_SHIFT_DC)

#define SSD1306_MEMORYMODE          0x20 ///< See datasheet
#define SSD1306_COLUMNADDR          0x21 ///< See datasheet
#define SSD1306_PAGEADDR            0x22 ///< See datasheet
#define SSD1306_SETCONTRAST         0x81 ///< See datasheet
#define SSD1306_CHARGEPUMP          0x8D ///< See datasheet
#define SSD1306_SEGREMAP            0xA0 ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON        0xA5 ///< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6 ///< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7 ///< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8 ///< See datasheet
#define SSD1306_DISPLAYOFF          0xAE ///< See datasheet
#define SSD1306_DISPLAYON           0xAF ///< See datasheet
#define SSD1306_COMSCANINC          0xC0 ///< Not currently used
#define SSD1306_COMSCANDEC          0xC8 ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3 ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 ///< See datasheet
#define SSD1306_SETPRECHARGE        0xD9 ///< See datasheet
#define SSD1306_SETCOMPINS          0xDA ///< See datasheet
#define SSD1306_SETVCOMDETECT       0xDB ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00 ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40 ///< See datasheet

#define SSD1306_EXTERNALVCC         0x10 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x14 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Set scroll range

#if defined SSD1306_128_64
 #define SSD1306_LCDWIDTH  128 ///width w/SSD1306_128_64 defined
 #define SSD1306_LCDHEIGHT  64 ///height w/SSD1306_128_64 defined
 #define SSD1306_COM_PINS  ((1<<4)|(0<<5))
 #define SSD1306_CONTRAST  0x8F
#endif
#if defined SSD1306_128_32
 #define SSD1306_LCDWIDTH  128 ///< DEPRECATED: width w/SSD1306_128_32 defined
 #define SSD1306_LCDHEIGHT  32 ///< DEPRECATED: height w/SSD1306_128_32 defined
 #define SSD1306_COM_PINS  0x02
 #define SSD1306_CONTRAST  0x8F
#endif
#if defined SSD1306_96_16
 #define SSD1306_LCDWIDTH   96 ///< DEPRECATED: width w/SSD1306_96_16 defined
 #define SSD1306_LCDHEIGHT  16 ///< DEPRECATED: height w/SSD1306_96_16 defined
#endif


#define BUFFER_HEIGHT 16 //SSD1306_LCDHEIGHT
#define BUFFER_WIDTH  16 // SSD1306_LCDWIDTH

static const uint8_t ssd1306_init_array [] = {
    SSD1306_DISPLAYOFF,                   // 0xAE
    SSD1306_SETDISPLAYCLOCKDIV,           // 0xD5
    0x80,                                 // the suggested ratio 0x80
    SSD1306_SETMULTIPLEX,
    0x3f, //SSD1306_LCDHEIGHT,
    SSD1306_SETDISPLAYOFFSET,             // 0xD3
    0x0,                                  // no offset
    SSD1306_SETSTARTLINE | 0x0,           // line #0
    SSD1306_CHARGEPUMP,
    SSD1306_SWITCHCAPVCC, // use internal VCC from +3.3
    SSD1306_MEMORYMODE,                   // 0x20
    0x00,                                 // 0x0 act like ks0108
    SSD1306_SEGREMAP | 0x1,
    SSD1306_COMSCANDEC,

    SSD1306_SETCOMPINS,
    SSD1306_COM_PINS,
    SSD1306_SETCONTRAST,
    SSD1306_CONTRAST,

    SSD1306_SETPRECHARGE,
    0xF1,
    SSD1306_SETVCOMDETECT,
    0x40,
    SSD1306_DISPLAYALLON_RESUME,         // 0xA4
    SSD1306_NORMALDISPLAY,               // 0xA6
    SSD1306_DEACTIVATE_SCROLL,
    //SSD1306_DISPLAYON
};

#ifdef SSD1306_GRAPH_MODE
static uint8_t video_buffer[BUFFER_WIDTH * BUFFER_HEIGHT/8];
#endif
static void ssd1306_display_buffer(uint8_t x_offset, uint8_t y_offset,
                                   uint8_t width, uint8_t height,
                                   const uint8_t *buf, uint8_t len);

static bool display_on = FALSE;

/**
 * @brief         screen initialization
 * @details       Send in screen driver array with initialization
 *                and memset by 0 screen area
 */
void ssd1306_init() {
#ifdef SSD1306_GRAPH_MODE
    ssd1306_clearDisplayBuffer();
#endif
    i2c_write_reg_array(SSD1306_I2C_ADDR,
                        SSD1306_COMMAND,
                        ssd1306_init_array,
                        sizeof(ssd1306_init_array));

    ssd1306_clear_display();
    ssd1306_display_on();

}
/**
 * @brief         if display is off enable it, do nothing otherwise
 * @details       function uses global driver state about display state
 */
void ssd1306_display_on() {
    static const uint8_t ssd1306_on_array [] = {
        SSD1306_DISPLAYON
    };

    if(!display_on) {
        i2c_write_reg_array(SSD1306_I2C_ADDR,
                            SSD1306_COMMAND,
                            ssd1306_on_array,
                            sizeof(ssd1306_on_array));
        display_on = TRUE;
    }
}
/**
 * @brief         if display is on enable it, do nothing otherwise
 * @details       function uses global driver state about display state
 */
void ssd1306_display_off() {
    static const uint8_t ssd1306_off_array [] = {
        SSD1306_DISPLAYOFF
    };
    if(display_on) {
        i2c_write_reg_array(SSD1306_I2C_ADDR,
                            SSD1306_COMMAND,
                            ssd1306_off_array,
                            sizeof(ssd1306_off_array));
        display_on = FALSE;
    }
}

#ifdef SSD1306_GRAPH_MODE
/**
 * @brief         set pixel in graph buffer
 * @param[in]     x - x coordinate in buffer
 * @param[in]     y - y coordinate in buffer
 * @param[in]     color - white, black or inverce
 *
 */
void ssd1306_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
  if((x < BUFFER_WIDTH) && (y < BUFFER_HEIGHT)) {
    // Pixel is in-bounds. Rotate coordinates if needed.
#if (SSD1306_SCREEN_ROTATION==1)
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
#elif (SSD1306_SCREEN_ROTATION==2)
      x = WIDTH  - x - 1;
      y = HEIGHT - y - 1;
#elif (SSD1306_SCREEN_ROTATION==2)
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
#endif

      switch(color) {
      case SSD1306_WHITE:
          video_buffer[x + (y/8)*BUFFER_WIDTH] |=  (1 << (y&7));
          break;
      case SSD1306_BLACK:
          video_buffer[x + (y/8)*BUFFER_WIDTH] &= ~(1 << (y&7));
          break;
      case SSD1306_INVERSE:
          video_buffer[x + (y/8)*BUFFER_WIDTH] ^=  (1 << (y&7));
          break;
      }
  }
}
/**
 * @brief         Clear all pixels in buffer
 */
void ssd1306_clearDisplayBuffer(void) {
    int i;
    for(i=0; i< sizeof(video_buffer); i++) {
        video_buffer[i] = 0x0;
    }
}
/**
 * @brief         synchronize display buffer and screen
 * @param[in]     x_offset - left coordinate where will
 *                           be displayed buffer content
 * @param[in]     y_offset - top coordinate where will
 *                           be displayed buffer content
 */
void ssd1306_display_video_buffer(uint8_t x_offset, uint8_t y_offset) {
    ssd1306_display_buffer(
        x_offset, y_offset,
        BUFFER_WIDTH, BUFFER_HEIGHT,
        video_buffer,
        sizeof(video_buffer));
}
#endif
/**
 * @brief         Clear display.
 * This function works a notable time
 */
void ssd1306_clear_display(void) {
    const uint8_t cmd_send_video_buffer[] = {
        SSD1306_PAGEADDR,
        0,
        SSD1306_LCDHEIGHT-1,
        SSD1306_COLUMNADDR,
        0,
        SSD1306_LCDWIDTH-1,
    };
    i2c_write_reg_array(SSD1306_I2C_ADDR,
                        SSD1306_COMMAND,
                        cmd_send_video_buffer,
                        sizeof(cmd_send_video_buffer));
    i2c_memset_reg_array(SSD1306_I2C_ADDR, SSD1306_DATA, 0x0, SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT);
}

/**
 * @brief         Draw char from specified font
 * @details       char is index in font table
 * @param[in]     ch - char index in font table
 * @param[in]     x - left coordinate
 * @param[in]     y - top coordinate
 */
void ssd1306_display_char(unsigned char ch, uint8_t x, uint8_t y) {
    uint8_t *buf;
    if(ch >= '0' && ch <= '9') {
        buf = font_table_digits[ch-'0'];
    } else if(ch >= 'a' && ch <= 'z') {
        buf = font_table_letters[ch-'a'];
    } else if(ch >= 'A' && ch <= 'Z') {
        buf = font_table_letters[ch-'A'];
    } else {
        buf = video_buffer;
    }

    ssd1306_display_buffer(
        x,
        y,
        FONT_WIDTH, FONT_HEIGHT,
        buf,
        FONT_WIDTH*FONT_HEIGHT/8);

}

static void ssd1306_display_buffer(uint8_t x_offset, uint8_t y_offset,
                                   uint8_t width, uint8_t height,
                                   const uint8_t *buf, uint8_t len) {
    uint8_t cmd_send_video_buffer[] = {
        SSD1306_PAGEADDR,
        0,
        0,
        SSD1306_COLUMNADDR,
        0,
        0,
        };

    cmd_send_video_buffer[1] = y_offset/8;
    cmd_send_video_buffer[2] = y_offset/8 + height/8-1;
    cmd_send_video_buffer[4] = x_offset;
    cmd_send_video_buffer[5] = x_offset + width - 1;
    i2c_write_reg_array(SSD1306_I2C_ADDR,
                        SSD1306_COMMAND,
                        cmd_send_video_buffer,
                        sizeof(cmd_send_video_buffer));

    i2c_write_reg_array(SSD1306_I2C_ADDR, SSD1306_DATA, buf, len);
}
