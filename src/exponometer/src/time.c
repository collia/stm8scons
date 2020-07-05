/**
 * @file    time.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Time supporting module
 *
 * For time meashuring with ms accuransy. Is used TIM4 timer
 */

#include "stm8s.h"
#include "stm8s_tim4.h"
#include "time.h"

/// global variable with current time value
volatile uint32_t time;

/**
 * @brief         init time module
 * @details       Configure TIM4 for tick period 1ms. Needs interrupts.
 */
void time_init() {
    /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - We need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 16);

  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);

}
/**
 * @brief         tim4 irq handler
 */
void time_irq_handler() {
    time++;
    TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
    //debug_blink_once();
}
