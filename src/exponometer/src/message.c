
#include "stm8s.h"
#include "time.h"
#include "message.h"

struct message {
    uint32_t time;
    uint8_t  flag; // 0 - message not ready
};
struct message messages[MESSAGE_NUMBER];


void message_pool_init() {
    uint8_t i;
    for(i = 0; i< MESSAGE_NUMBER; i++) {
        messages[i].time = 0;
        messages[i].flag = 0;
    }
}
void message_send(uint8_t msg, uint32_t delay) {
    if(msg < MESSAGE_NUMBER) {
        if(time > messages[msg].time) {
               messages[msg].flag = 1;
               messages[msg].time = time+delay;
        }
    }
}
void message_resend(uint8_t msg, uint32_t delay) {
    if(msg < MESSAGE_NUMBER) {
        messages[msg].flag = 1;
        messages[msg].time = time+delay;
    }
}
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
uint32_t message_get_msg_time(uint8_t msg) {
    return messages[msg].time;
}
