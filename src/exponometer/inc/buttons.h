#ifndef _BUTTONS_H_ 
#define _BUTTONS_H_

void buttons_init();
void buttons_irq_handler();
void button_set_irq_en(bool en);
void buttons_send_status();

#endif //_BUTTONS_H_
