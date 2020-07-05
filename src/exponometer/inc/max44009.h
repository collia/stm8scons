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

#ifndef _MAX44009_H_
#define  _MAX44009_H_
#include "stm8s.h"

void max44009_init();
int16_t max44009_get_lux();
#endif
