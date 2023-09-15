
#include "utility.h"

//void delay_us(uint32_t us)
//{
//    uint32_t i,j;
//    for(j=0; j<us; j++)
//    {
//        for(i=0; i<10; i++)
//        {
//            __asm("NOP");
//        }
//        __asm("NOP");
//        __asm("NOP");
//        __asm("NOP");
//        __asm("NOP");
//        __asm("NOP");
//        __asm("NOP");
////		__ASM("NOP");
////		__ASM("NOP");
//    }
//}


//void delay_ms(uint32_t ms)
//{
//    uint32_t j;
//    for(j=0; j<ms; j++)
//        delay_us(880);
//}


/******************spi delay**************************/
void delay_us(uint32_t us)
{
    uint32_t i,j;
    for(j=0; j<us; j++)
        for(i=0; i<2; i++);
}


void delay_ms(uint32_t ms)
{
    uint32_t j;
    for(j=0; j<ms; j++)
        delay_us(763);
}
