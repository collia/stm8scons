/**
 * @file    battery.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   File contains sources for batary meashurer
 *
 * In schematic is connected 1.2 stabilized source to ADC port.
 * So, when power level is changing, reading from ADC the same value
 * will return different values.
 * Algorithm of calculation is table 'power_values' with
 * premeashured values
 */

#include "stm8s.h"
#include "stm8s_adc1.h"
#include "stm8s_gpio.h"

typedef struct {
    uint16_t adc;
    uint8_t v;
} power_convert;

/**
 * @brief         Array with premeashured values
 * @details       In calibration mode is shown current voltage level
 */
static const power_convert power_values[] = {
    {382, 33},
    {394, 32},
    {406, 31},
    {419, 30},
    {435, 29},
    {450, 28},
    {469, 27},
};

static uint8_t get_battery_state(uint16_t adc_val) {
    uint8_t i;
    //const uint16_t ref_v = 122;
    //return ((uint16_t)(1025*ref_v)/adc_val);
    for(i=0; i < sizeof(power_values); i++) {
        if(adc_val < power_values[i].adc) {
            return power_values[i].v;
        }
    }
    return 0;
}

void battery_init() {
    ADC1_DeInit();
    /*  Init GPIO for ADC2 */
    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);

    /* Init ADC2 peripheral */
    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_6,
              ADC1_PRESSEL_FCPU_D18,
              ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT,
              ADC1_SCHMITTTRIG_CHANNEL6,
              DISABLE);
    ADC1_Cmd(ENABLE);
}

bool battery_get_voltage(uint8_t* result) {
     if(ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
        *result = get_battery_state(
            ADC1_GetConversionValue());
        ADC1_ClearFlag(ADC1_FLAG_EOC);
        ADC1_Cmd(ENABLE);
        return TRUE;
     }
     ADC1_Cmd(ENABLE);

     return FALSE;
}
