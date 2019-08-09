#ifndef HH__XSYS_SPIFLASH__HH
#define HH__XSYS_SPIFLASH__HH

/*------------------------------------------------------------------------------------------------------------------
包含头文件
*/
#include "stdint.h"


/*------------------------------------------------------------------------------------------------------------------
宏定义
*/



/*------------------------------------------------------------------------------------------------------------------
函数声明
*/
void xSYS_SpiFlash_Init(void);
void xSYS_SpiFlash_EraseSector(uint32_t SectorAddr);
void xSYS_SpiFlash_EraseChip(void);
void xSYS_SpiFlash_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void xBSP_SpiFlash_Write_NoCheck(uint8_t* pBuff,uint32_t writeAddr,uint16_t numByteToWrite);
void xSYS_SpiFlash_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint8_t xSYS_SpiFlash_CheckUse(void);

uint8_t xSYS_SpiFlash_Test(void);

#endif

/*************************** End of file ****************************/
