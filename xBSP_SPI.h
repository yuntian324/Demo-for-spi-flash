/**************************************************START OF FILE*****************************************************/
#ifndef HH__XBSP_SPI__HH
#define HH__XBSP_SPI__HH

#include "stdint.h"

/*------------------------------------------------------------------------------------------------------------------
宏定义
*/
#define SPI_EN_CRITICAL 1//SPI FLASH关键字段设置临界区


#define SPI1_CS_MAIN(n)                 (n ? (PAout(4)=1) : (PAout(4)=0))
#define SPI1_CS_C489(n)                 (n ? (PBout(0)=1) : (PBout(0)=0))
#define SPI1_C489_UPDATEEN(n)           (n ? (PBout(8)=1) : (PBout(8)=0))//74HC157切换

#define SPI2_CS_595(n)                  (n ? (PBout(12)=1) : (PBout(12)=0))
#define SPI2_PL_165(n)                  (n ? (PCout(5)=1) : (PCout(5)=0))

#define SPI1_SEL_MAIN                   {SPI1_CS_MAIN(0); SPI1_CS_C489(1); SPI1_C489_UPDATEEN(0);}
#define SPI1_SEL_C489                   {SPI1_CS_MAIN(1); SPI1_CS_C489(0); SPI1_C489_UPDATEEN(1);}
#define SPI1_SEL_NULL                   {SPI1_CS_MAIN(1); SPI1_CS_C489(0); SPI1_C489_UPDATEEN(0);}

#define SPI2_SEL_595                    {SPI2_CS_595(0);}
#define SPI2_SEL_NULL                   {SPI2_CS_595(1);}

/*------------------------------------------------------------------------------------------------------------------
函数声明
*/
void xBSP_SPI1_Init(void);
void xBSP_SPI2_Init(void);
uint8_t xBSP_SPI1_WriteReadByte(uint8_t Byte);
uint8_t xBSP_SPI2_WriteReadByte(uint8_t Byte);


#endif
/**************************************************END OF FILE**********************************************************/

