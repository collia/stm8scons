#include <stdlib.h>
#include <string.h>

#include "ssd1306.h"
#include "stm8s.h"
#include "i2c.h"

// Configuration

#define SSD1306_I2C_ADDR   (0x78)

#define SSD1306_128_64


#define ssd1306_swap(a, b) \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

#define getRotation()  (0)


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

#define SSD1306_EXTERNALVCC         0x01 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

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
 #define SSD1306_COM_PINS  0x02
 #define SSD1306_CONTRAST  0x8F
#endif
#if defined SSD1306_128_32
 #define SSD1306_LCDWIDTH  128 ///< DEPRECATED: width w/SSD1306_128_32 defined
 #define SSD1306_LCDHEIGHT  32 ///< DEPRECATED: height w/SSD1306_128_32 defined
#endif
#if defined SSD1306_96_16
 #define SSD1306_LCDWIDTH   96 ///< DEPRECATED: width w/SSD1306_96_16 defined
 #define SSD1306_LCDHEIGHT  16 ///< DEPRECATED: height w/SSD1306_96_16 defined
#endif

#define HEIGHT SSD1306_LCDHEIGHT
#define WIDTH  SSD1306_LCDWIDTH

static const uint8_t ssd1306_init_array [] = {
    0, // Co = 0, D/C = 0
    SSD1306_DISPLAYOFF,                   // 0xAE
    SSD1306_SETDISPLAYCLOCKDIV,           // 0xD5
    0x80,                                 // the suggested ratio 0x80
    SSD1306_SETMULTIPLEX,
    SSD1306_LCDHEIGHT,
    SSD1306_SETDISPLAYOFFSET,             // 0xD3
    0x0,                                  // no offset
    SSD1306_SETSTARTLINE | 0x0,           // line #0
    SSD1306_CHARGEPUMP,
    // use internal VCC from +3.3
    SSD1306_SWITCHCAPVCC,
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
    SSD1306_DISPLAYON
};


static uint8_t cmd_mem_buffer[7+(WIDTH * ((HEIGHT + 7) / 8))] = {
    SSD1306_PAGEADDR,
    0,                         // Page start address
    0xFF,                      // Page end (not really, but works here)
    SSD1306_COLUMNADDR,
    0,
    WIDTH-1,
    0x40
};
//static uint8_t * const buffer = cmd_mem_buffer+7;

void ssd1306_init() {
        //buffer[1] = 1;
        //buffer[2] = 2;
        //buffer[3] = 3;
        //buffer[4] = 4;
        //buffer[5] = 5;

    cmd_mem_buffer[7+10] = 0xa;
    cmd_mem_buffer[7+11] = 0xb;
    cmd_mem_buffer[7+12] = 0xc;
    cmd_mem_buffer[7+13] = 0xd;

    i2c_write_array(SSD1306_I2C_ADDR,
                    ssd1306_init_array,
                    sizeof(ssd1306_init_array));
    ssd1306_clearDisplay();
    
}



void ssd1306_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
  if((x >= 0) && (x < WIDTH) && (y >= 0) && (y < HEIGHT)) {
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch(getRotation()) {
     case 1:
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
      break;
     case 2:
      x = WIDTH  - x - 1;
      y = HEIGHT - y - 1;
      break;
     case 3:
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
      break;
    }
    switch(color) {
     case SSD1306_WHITE:   cmd_mem_buffer[7+x + (y/8)*WIDTH] |=  (1 << (y&7)); break;
     case SSD1306_BLACK:   cmd_mem_buffer[7+x + (y/8)*WIDTH] &= ~(1 << (y&7)); break;
     case SSD1306_INVERSE: cmd_mem_buffer[7+x + (y/8)*WIDTH] ^=  (1 << (y&7)); break;
    }
  }
}

void ssd1306_clearDisplay(void) {
    int i;
    //for(i=0; i< WIDTH * ((HEIGHT + 7) / 8); i++)
    //    cmd_mem_buffer[7+i] = 0;
        // memset(&cmd_mem_buffer[7], 0xff, WIDTH * ((HEIGHT + 7) / 8));
}


void ssd1306_display(void) {

    i2c_write_array(SSD1306_I2C_ADDR, cmd_mem_buffer, sizeof(cmd_mem_buffer));
}
