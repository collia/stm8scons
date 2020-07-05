/**
 * @file    debug.h
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   File contains API for early stage debug,
 *          main idea to blink with GPIO with different
 *          frequency
 *
 * Disabled for space optimization
 */

#ifndef DEBUG_H_
#define DEBUG_H_


void debug_init();
void debug_blink_fast_forever();
void debug_blink_slow_forever();
void debug_blink_1_sec();
void debug_set_once(uint8_t en);

#endif
