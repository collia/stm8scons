/**
 * @file    max44009.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Driver for light sensor MAX 44009
 *
 * Driver works with light sensor connected by i2c 
 * with address 0x94.
 * external interrup connected to a.3, but it is not used now
 */

#include "max44009.h"
#include "stm8s.h"
#include "i2c.h"

#define MAX44009_ADDR (0x94)
//0x94
//(0x7B)

#define MAX44009_INTR_STATUS    0
#define MAX44009_INTR_ENABLE    1
#define MAX44009_CONFIG         2

#define MAX44009_LUX_HIGHT      3
#define MAX44009_LUX_LOW        4

#define MAX44009_TRESHOLD_UP    5
#define MAX44009_TRESHOLD_LOW   6
#define MAX44009_TRESHOLD_TME   7

/**
 * @brief         initialization
 * @details       configure GPIO A.3 for input mode
 *                this port connected to light sensor inrrupt line
 */
void max44009_init() {
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA,
                              EXTI_SENSITIVITY_RISE_FALL);

    GPIO_Init(EXTI_PORT_GPIOA,
              GPIO_PIN_3,
              GPIO_MODE_IN_PU_IT);

}

/**
 * @brief         Read sensor lux
 *
 * @return        return vlue in format
 *                  e3 e2 e1 e0 m7 m6 m5 m4 m3 m2 m1 m0
 *                 where e - bits for exponent
 *                       m - bits for mantissa
 *                 or -1 for not sucess
 */
int16_t max44009_get_lux() {
    int16_t res;
    uint8_t low ;
    uint8_t hi;
    int rc;
    // Lux = 2^(EXP) * Mantissa * 0.045
    // EXP = HIGHT[7:4]
    // Mantissa = HIGHT[3:0] LOW[3:0]
    rc = i2c_read_byte(MAX44009_ADDR, MAX44009_LUX_HIGHT);
    if(rc < 0)
        return -1;
    hi = rc & 0xff;
    rc = i2c_read_byte(MAX44009_ADDR, MAX44009_LUX_LOW);
    if(rc < 0)
        return -1;
    low = rc & 0xff;
    res = ((uint16_t)hi)<<4 | low & 0xf;
    return res;
}

