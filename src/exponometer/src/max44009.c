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



void max44009_init() {
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA,
                              EXTI_SENSITIVITY_RISE_FALL);

    GPIO_Init(EXTI_PORT_GPIOA,
              GPIO_PIN_3,
              GPIO_MODE_IN_PU_IT);

}


uint16_t max44009_get_lux() {
    uint16_t res;
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

