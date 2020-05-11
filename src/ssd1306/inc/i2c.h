#ifndef _I2C_H_
#define _I2C_H_

//#include <stdint.h>
#include "stm8s.h"

int i2c_master_init();
int i2c_write_array(uint8_t dev, uint8_t* data, uint16_t length);
int i2c_read_word(uint8_t dev);
void i2c_irq_handler();
#endif
