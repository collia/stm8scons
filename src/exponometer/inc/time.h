/**
 * @file    time.h
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Time supporting module
 *
 * For time meashuring with ms accuransy. Is used TIM4 timer
 */

#ifndef _TIME_H_
#define _TIME_H_

void time_init();
void time_irq_handler();

/// global variable with current time value
volatile extern uint32_t time;


#endif //_TIME_H_
