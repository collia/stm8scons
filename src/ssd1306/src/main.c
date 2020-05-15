#include "stm8s.h"
#include "stm8s_gpio.h"
#include "i2c.h"
#include "debug.h"
#include "ssd1306.h"

void clock_setup(void);

int main()
{
    int x = 0;
    int y = 0;
    int offset_x = 0;
    int offset_y = 0;
    unsigned char ch = '0';
    debug_init();

    clock_setup();
    i2c_master_init();
    //enableInterrupts();
    ssd1306_init();

    while(1) {
        int i = 0;
        for(i = 0; i < 16000; i++);
        //debug_blink_1_sec();
#if 0
        ssd1306_drawPixel(x, y, SSD1306_WHITE);
        x++;
        if(x >16) {
            x = 0;
            y++;
        }
        if(y >16) {
            y = 0;
            offset_x += 16;
            ssd1306_clearDisplayBuffer();
        }
        if(offset_x > 128-16) {
            offset_x = 0;
            offset_y += 16;
        }

        ssd1306_display_video_buffer(offset_x, offset_y);
#else

        ssd1306_display_char(ch, offset_x, offset_y);
        ch++;
        if((ch >'9') && (ch <'A'))
            ch = 'A';
        else if((ch >'Z') && (ch <'a'))
            ch = 'a';
        else if(ch >'z')
            ch = '0';

        offset_x += 16;
        if(offset_x > 128-16) {
            offset_x = 0;
            offset_y += 16;
        }
        if(offset_y > 64-16) {
            offset_y = 0;
        }

#endif
    }
}

void clock_setup(void)

{
      CLK_DeInit();

      CLK_HSECmd(DISABLE);
      CLK_LSICmd(DISABLE);
      CLK_HSICmd(ENABLE);
      while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
      CLK_ClockSwitchCmd(ENABLE);
      CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
      CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV2);
      CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
                            DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}


/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
    debug_blink_slow_forever();
}
