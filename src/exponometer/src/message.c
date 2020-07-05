/**
 * @file    message.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-05
 * @brief   Message interruction routine
 *
 */

#include "stm8s.h"
#include "time.h"
#include "message.h"

struct message {
    uint32_t time;
    uint8_t  flag; // 0 - message not ready
};
struct message messages[MESSAGE_NUMBER];

/**
 * @brief         initialize messages pool
 */
void message_pool_init() {
    uint8_t i;
    for(i = 0; i< MESSAGE_NUMBER; i++) {
        messages[i].time = 0;
        messages[i].flag = 0;
    }
}
/**
 * @brief         Send message
 * @details       send message or immediately or with delay
 * @param[in]     msg - message
 * @param[in]     delay - delay in ms, time between message
 *                sending and reseiving
 */
void message_send(uint8_t msg, uint32_t delay) {
    if(msg < MESSAGE_NUMBER) {
        if(time > messages[msg].time) {
               messages[msg].flag = 1;
               messages[msg].time = time+delay;
        }
    }
}
/**
 * @brief         Resend same message
 * @details       Remove from queue previous message and place new
 *                can be used for increase delay
 * @param[in]     msg - message
 * @param[in]     delay - delay in ms when message can be reseived
 *
 */
void message_resend(uint8_t msg, uint32_t delay) {
    if(msg < MESSAGE_NUMBER) {
        messages[msg].flag = 1;
        messages[msg].time = time+delay;
    }
}
/**
 * @brief         reseive newest message from pool
 *
 * @return        latest message, or MESSAGE_NUMBER if no messages
 */
uint8_t message_resv() {
    uint8_t i;
    for(i = 0; i< MESSAGE_NUMBER; i++) {
        if(messages[i].flag && (messages[i].time < time) ) {
            messages[i].flag = 0;
            return i;
        }
    }
    return MESSAGE_NUMBER;
}
/**
 * @brief         Get time when message should be reseived,
 *                or was reseived latest
 * @param[in]     msg message
 *
 * @return        time when will be reseived message,
 *                or when it was reseived
 */
uint32_t message_get_msg_time(uint8_t msg) {
    return messages[msg].time;
}
