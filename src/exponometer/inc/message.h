#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define MESSAGE_MODE_BUTTON        (0)
#define MESSAGE_UP_BUTTON          (1)
#define MESSAGE_DOWN_BUTTON        (2)
#define MESSAGE_BUTTONS_RELEASED   (3)
#define MESSAGE_BUTTONS_IRQ        (4)

#define MESSAGE_NUMBER             (5)


void message_pool_init();
void message_send(uint8_t msg, uint8_t delay);
uint32_t message_get_msg_time(uint8_t msg);
uint8_t message_resv();
#endif
