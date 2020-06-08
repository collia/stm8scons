#ifndef _I2C_H_
#define _I2C_H_

//#include <stdint.h>
#include "stm8s.h"

int i2c_master_init();
int i2c_write_reg_array(uint8_t dev, uint8_t reg, uint8_t* data, uint16_t length);
int i2c_memset_reg_array(uint8_t dev, uint8_t reg, uint8_t val, uint16_t length);

int i2c_read_byte(uint8_t dev, uint8_t reg);
#endif
