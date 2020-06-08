#include "i2c.h"
#include "stm8s_i2c.h"
#include "stm8s_gpio.h"
#include "debug.h"

#define BUFFERSIZE  10
#define I2C_SPEED 100000

#define I2C_WAIT_FLAG_TIMEOUT(cond) {\
    int _i=3200;\
    while((cond) && _i) { \
    if(--_i == 0) return -1;\
    };\
    }
#define I2C_WAIT_FLAG(cond) {\
    while(cond);\
    }

int i2c_master_init() {
    uint8_t i = 0;

    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);

    //I2C_DeInit();
    /* I2C Initialize */
    I2C_Init(I2C_SPEED,
             0x78,
             I2C_DUTYCYCLE_2,
             I2C_ACK_CURR,
             I2C_ADDMODE_7BIT,
             16
             /*CLK_GetClockFreq() / 1000000*/);

    /* Enable Buffer and Event Interrupt*/
    //I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR) , ENABLE);
    I2C_Cmd(ENABLE);
    return 0;
}

int i2c_read_byte(uint8_t dev, uint8_t reg) {

   uint8_t byte = 0x00;

   while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));

   I2C_GenerateSTART(ENABLE);
   I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
   I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
   I2C_Send7bitAddress(dev, I2C_DIRECTION_TX);
   I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
   I2C_SendData(reg);

   I2C_GenerateSTART(ENABLE);
   I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
   I2C_Send7bitAddress(dev, I2C_DIRECTION_RX);
   I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
   I2C_AcknowledgeConfig(I2C_ACK_NONE);
   I2C_WAIT_FLAG_TIMEOUT(!(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)));
   
   byte = I2C_ReceiveData();

   I2C_GenerateSTOP(ENABLE);
   I2C_AcknowledgeConfig(I2C_ACK_CURR);
   return byte;
}


int i2c_write_reg_array(uint8_t dev, uint8_t reg, uint8_t* data, uint16_t length) {

    int bytes_sent = 0;
    I2C_GenerateSTART(ENABLE);
    I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(dev, I2C_DIRECTION_TX);
    I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(reg);
    I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    for(bytes_sent = 0; bytes_sent < length; bytes_sent++) {
        I2C_SendData(data[bytes_sent]);
        I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    I2C_GenerateSTOP(ENABLE);
    return bytes_sent;
}

int i2c_memset_reg_array(uint8_t dev, uint8_t reg, uint8_t val, uint16_t length) {

    int bytes_sent = 0;
    I2C_GenerateSTART(ENABLE);
    I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(dev, I2C_DIRECTION_TX);
    I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(reg);
    for(bytes_sent = 0; bytes_sent < length; bytes_sent++) {
        I2C_SendData(val);
        I2C_WAIT_FLAG(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    I2C_GenerateSTOP(ENABLE);
    return bytes_sent;
}

