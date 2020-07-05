/**
 * @file    battery.h
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

#ifndef _BATTERY_H_
#define _BATTERY_H_

void battery_init();
bool battery_get_voltage(uint8_t* result);

#endif
