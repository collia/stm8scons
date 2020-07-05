/**
 * @file    buttons.h
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   File contains functions for buttons handlings
 *
 * Buttons connected for C.4 C.5 C.6
 * Interrups should be enabled
 */

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

void buttons_init();
void buttons_irq_handler();
void button_set_irq_en(bool en);
void buttons_send_status();

#endif //_BUTTONS_H_
