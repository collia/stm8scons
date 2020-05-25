#ifndef _TIME_H_
#define _TIME_H_

void time_init();
void time_irq_handler();
volatile extern uint32_t time;
    

#endif //_TIME_H_
