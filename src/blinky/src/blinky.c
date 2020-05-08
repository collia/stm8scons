#include "stm8s.h"
#include "stm8s_gpio.h"
int main() 
{
   int d;
   // Configure pins
   GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
   // Loop
   do {
       GPIO_WriteReverse(GPIOB, GPIO_PIN_5);
       
       for(d = 0; d<24000; d++);
      } while(1);
}


/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
