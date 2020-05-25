#include "stm8s.h"
//#include "stm8s_gpio.h"
#include "i2c.h"
#include "debug.h"
#include "ssd1306.h"
#include "terminal.h"
#include "max44009.h"
#include "buttons.h"
#include "time.h"
#include "message.h"
#include "stm8s_it.h"


static void clock_setup(void);
static void continius_mode();

static void init_calibration(void);
static void init_exposure(void);
static void init_delay(void);

static void run_calibration(void);
static void run_exposure(void);
static void run_delay(void);

typedef enum {
    MODE_CALIBRATION,
    MODE_MEASHURE_EXPOSURE,
    MODE_MEASHURE_DELAY,
    MODE_MAX,
} device_mode;

typedef void (*hadler)(void);

static const hadler init_handlers[MODE_MAX] = {
    init_calibration,
    init_exposure,
    init_delay
};
static const hadler run_handlers[MODE_MAX] = {
    run_calibration,
    run_exposure,
    run_delay
};

int main()
{
    device_mode mode = MODE_CALIBRATION;
    uint8_t msg, prev_btn_msg = MESSAGE_BUTTONS_RELEASED;
    
    debug_init();
    clock_setup();
    time_init();
    i2c_master_init();
    buttons_init();

    enableInterrupts();

    ssd1306_init();
    max44009_init();

    init_handlers[mode]();
    while(1) {
        msg = message_resv();
        if(msg == MESSAGE_BUTTONS_RELEASED) {
            switch(prev_btn_msg) {
            case MESSAGE_MODE_BUTTON:
                //term_print("MS", 4, 0);
                if(++mode >= MODE_MAX) {
                    mode = 0;
                }
                init_handlers[mode]();
                break;
            case MESSAGE_UP_BUTTON:
                //term_print("US", 4, 0);
                break;
            case MESSAGE_DOWN_BUTTON:
                //term_print("DS", 4, 0);
                break;
            }
            //if(message_get_msg_time(msg) -
            //   message_get_msg_time(prev_btn_msg) > 2000) {
            //    term_print("L", 5, 0);
            //}
        } else if(msg == MESSAGE_MODE_BUTTON ||
                  msg == MESSAGE_UP_BUTTON ||
                  msg == MESSAGE_DOWN_BUTTON) {
            prev_btn_msg = msg;
        } else if(msg == MESSAGE_BUTTONS_IRQ) {
            buttons_send_status();
            button_set_irq_en(1);
        }

        run_handlers[mode]();
    }
}

static void clock_setup(void) {
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
      CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
}

static void init_calibration(void) {
    term_print("CAL", 0, 0);
    term_print("LUX", 0, 1);
    term_print("---E--",0, 2);
}
static void init_exposure(void) {
    term_print("EXP", 0, 0);
}
static void init_delay(void) {
    term_print("DEL", 0, 0);
}

static void run_calibration() {
    static int i = 0;
    int lux;
    //for(i = 0; i < 24000; i++);
    //debug_blink_1_sec();
    if(i++ < 1600) {
        return;
    }
    i = 0;
    lux = max44009_get_lux();
    if(lux < 0) {
        term_print("---E--", 0, 2);
    } else {
        term_print_int(
            lux & 0xff,
            0, 2, 3);
        term_print_int(
            (lux & 0xf00) >> 8,
            4, 2, 2);
    }
}
static void run_exposure() {
}
static void run_delay() {
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
