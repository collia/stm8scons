#include "stm8s_it.h"
//#include "main.h"
#include "i2c.h"
#include "buttons.h"
#include "time.h"

INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
}

INTERRUPT_HANDLER(TLI_IRQHandler, 0)
{
}

INTERRUPT_HANDLER(AWU_IRQHandler, 1)	
{
}

INTERRUPT_HANDLER(CLK_IRQHandler, 2)
{
}

INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
{
}

INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
}

INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
    buttons_irq_handler();
}

INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
}

INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
}
INTERRUPT_HANDLER(SPI_IRQHandler, 10)
{
}

INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
}

INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
{
}

INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{
}

INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
{
}


#if defined(STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined(STM8S003) || defined(STM8S001) || defined(STM8AF62Ax) || defined(STM8AF52Ax) || defined(STM8S903)
 INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
}

 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
}
#endif /*STM8S208, STM8S207 or STM8S103 or STM8AF62Ax or STM8AF52Ax or STM8S903 */

INTERRUPT_HANDLER(I2C_IRQHandler, 19)
{
}
 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
{
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
    time_irq_handler();
}

INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
{
}

