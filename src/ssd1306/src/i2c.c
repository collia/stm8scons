#include "i2c.h"
#include "stm8s_i2c.h"
#include "stm8s_gpio.h"
#include "debug.h"

#define BUFFERSIZE  10
#define I2C_SPEED 100000
#if 0
static uint8_t current_slave_addr = 0;
static __IO uint8_t RxBuffer[BUFFERSIZE];
static __IO uint8_t TxBuffer[BUFFERSIZE];
static __IO uint8_t NumOfBytes;
static __IO uint8_t Tx_Idx;
#endif
int i2c_master_init() {
    uint8_t i = 0;

    GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);

    I2C_DeInit();
    /* I2C Initialize */
    I2C_Init(I2C_SPEED,
             0x78,
             I2C_DUTYCYCLE_2,
             I2C_ACK_CURR,
             I2C_ADDMODE_7BIT,
             CLK_GetClockFreq() / 1000000);

    /* Enable Buffer and Event Interrupt*/
    I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR) , ENABLE);
    I2C_Cmd(ENABLE);
    return 0;
}

int i2c_read_word(uint8_t dev){
#if 0
   unsigned long value = 0x0000;
   unsigned char num_of_bytes = 0x01;
   unsigned char bytes[2] = {0x00, 0x00};

   while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));

   I2C_GenerateSTART(ENABLE);
   while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

   I2C_Send7bitAddress(dev, I2C_DIRECTION_RX);
   while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

   while(num_of_bytes)
   {
        if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
       {
            if(num_of_bytes == 0)
            {
                 I2C_AcknowledgeConfig(I2C_ACK_NONE);
                 I2C_GenerateSTOP(ENABLE);
             }
            bytes[(num_of_bytes - 1)] = I2C_ReceiveData();
            num_of_bytes--;
         }
   };
   value = ((bytes[1] << 8) | bytes[0]);
   return value;
#endif
   return dev;
}


int i2c_write_reg_array(uint8_t dev, uint8_t reg, uint8_t* data, uint16_t length) {

    int bytes_sent = 0;
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(dev, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(reg);
    for(bytes_sent = 0; bytes_sent < length; bytes_sent++) {
        I2C_SendData(data[bytes_sent]);
        while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    I2C_GenerateSTOP(ENABLE);
    return bytes_sent;
}

int i2c_memset_reg_array(uint8_t dev, uint8_t reg, uint8_t val, uint16_t length) {

    int bytes_sent = 0;
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(dev, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(reg);
    for(bytes_sent = 0; bytes_sent < length; bytes_sent++) {
        I2C_SendData(val);
        while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    I2C_GenerateSTOP(ENABLE);
    return bytes_sent;
}


void i2c_irq_handler() {
#if 0
    //debug_blink_fast_forever();
    switch (I2C_GetLastEvent())
    {
        /* EV5 */
    case I2C_EVENT_MASTER_MODE_SELECT :

        /* Send slave Address for write */
        I2C_Send7bitAddress(current_slave_addr, I2C_DIRECTION_TX);
        break;
        /* EV6 */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
        if (NumOfBytes != 0)
        {
            /* Send the first Data */
            I2C_SendData(TxBuffer[Tx_Idx++]);

            /* Decrement number of bytes */
            NumOfBytes--;
        }
        if (NumOfBytes == 0)
        {
            I2C_ITConfig(I2C_IT_BUF, DISABLE);
        }
        break;

        /* EV8 */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
        /* Transmit Data */
        I2C_SendData(TxBuffer[Tx_Idx++]);

        /* Decrement number of bytes */
        NumOfBytes--;

        if (NumOfBytes == 0)
        {
            I2C_ITConfig(I2C_IT_BUF, DISABLE);
        }
        break;

        /* EV8_2 */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
        /* Send STOP condition */
        I2C_GenerateSTOP(ENABLE);

        I2C_ITConfig(I2C_IT_EVT, DISABLE);
        break;

    default:
        break;
    }
#endif
}
