/**************************************************START OF FILE*****************************************************/
/**********************************************************************************************
文 件 名: SpiFlash.c

文件备注: SPI FLASH驱动函数,不涉及项目逻辑

作    者: WZB
***********************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------
包含头文件
*/
#include "stm32f2xx_hal.h"
#include "xSYS_SpiFlash.h"
#include "xBSP_SPI.h"
#include "xBSP_IO_Map.h"

/*------------------------------------------------------------------------------------------------------------------
宏定义
*/
#define  sFLASH_PAGESIZE        0x100//256Byte
#define  sFLASH_SECTORSIZE      0x1000//4KByte
#define  sFLASH_BLOOKSIZE       0X10000//64KByte

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define M25P64_FLASH_ID        0x202017//C22017
#define M25P40_FLASH_ID        0x202013


/* USER CODE BEGIN Includes */
#define sFLASH_CMD_WRITE				0x02  /*!< Write to Memory instruction */
#define sFLASH_CMD_WRSR					0x01  /*!< Write Status Register instruction */
#define sFLASH_CMD_WREN					0x06  /*!< Write enable instruction */
#define sFLASH_CMD_READ					0x03  /*!< Read from Memory instruction */
#define sFLASH_CMD_RDSR					0x05  /*!< Read Status Register instruction  */
#define sFLASH_CMD_SE					0x20  /*!< Sector Erase instruction (4k)*/
#define sFLASH_CMD_BE					0xD8  /*!< Block Erase instruction (64k)*/
#define sFLASH_CMD_CE					0xC7  /*!< Chip Erase instruction (Chip Erase)*/
#define sFLASH_WIP_FLAG					0x01  /*!< Write In Progress (WIP) flag */
#define sFLASH_CMD_DeviceID				0xAB 
#define sFLASH_CMD_ManufactDeviceID		0x90 //Read Manufacturer / Device ID
#define sFLASH_CMD_IdentificationID		0x9F //JEDEC ID 

#define sFLASH_DUMMY_BYTE				0xFF

//**硬件宏定义**
#define CS_SpiFlash_LOW                 SPI1_SEL_MAIN    //SPI FLASH CS引脚拉低，片选使能
#define CS_SpiFlash_HIGH                SPI1_SEL_NULL  //SPI FLASH CS引脚拉高，片选失能

/*------------------------------------------------------------------------------------------------------------------
变量声明
*/
static volatile uint8_t SpiFlashBusyFlag = 0;//SPI FLASH忙碌标志，0-无操作SPI FLASH;非0-正在操作SPI FLASH;
static uint8_t sFlashBuff[4096];

/*------------------------------------------------------------------------------------------------------------------
函数声明
*/
static void xBSP_SpiFlash_WriteEnable(void);
static uint8_t xBSP_SpiFlash_SendByte(uint8_t byte);
static void xBSP_SpiFlash_WaitForWriteEnd(void);
void xBSP_SpiFlash_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
uint16_t xBSP_SpiFlash_Read_ManufactDeviceID(void);
uint32_t xBSP_SpiFlash_Read_IdentificationID(void);

/*
***************************************************************************************************************
@ Name   : xSYS_SpiFlash_Init

@ Brief  : SPI FLASH初始化配置

@ Param  : NULL

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13
***************************************************************************************************************
*/
void xSYS_SpiFlash_Init(void)
{
	SpiFlashBusyFlag = 0;
	xBSP_SPI1_Init();
	xBSP_SpiFlash_Read_ManufactDeviceID();
//	xSYS_SpiFlash_Test();	
}


/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_WriteEnable

@ Brief  : 发送命令，使能SPI FLASH写操作

@ Param  : NULL

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13
***************************************************************************************************************
*/
static void xBSP_SpiFlash_WriteEnable(void)
{
	CS_SpiFlash_LOW;

	xBSP_SpiFlash_SendByte(sFLASH_CMD_WREN);

	CS_SpiFlash_HIGH;
}

/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_WaitForWriteEnd

@ Brief  : SPI Flash操作，等待写入结束

@ Param  : NULL

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13
***************************************************************************************************************
*/
void xBSP_SpiFlash_WaitForWriteEnd(void)
{
    uint8_t flashstatus = 0;

    CS_SpiFlash_LOW;
    xBSP_SpiFlash_SendByte(sFLASH_CMD_RDSR);

    do
    {
		flashstatus = xBSP_SpiFlash_SendByte(sFLASH_DUMMY_BYTE);
    }
    while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */

    CS_SpiFlash_HIGH;
}

/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_SendByte

@ Brief  : SPI Flash操作，写入一个字节数据

@ Param  : uint8_t byte：要写入的数据

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13
***************************************************************************************************************
*/
uint8_t xBSP_SpiFlash_SendByte(uint8_t byte)
{
    return xBSP_SPI1_WriteReadByte(byte);
}

/*
***************************************************************************************************************
@ Name   : xSYS_SpiFlash_EraseSector

@ Brief  : SPI Flash操作，擦除指定扇区（4KByte）

@ Param  : uint32_t SectorAddr:扇区地址 根据实际容量设置

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13
***************************************************************************************************************
*/
void xSYS_SpiFlash_EraseSector(uint32_t SectorAddr)
{
	SpiFlashBusyFlag += 1;
	xBSP_SpiFlash_WriteEnable();
	CS_SpiFlash_LOW;
	xBSP_SpiFlash_SendByte(sFLASH_CMD_SE);
	xBSP_SpiFlash_SendByte((SectorAddr & 0xFF0000) >> 16);
	xBSP_SpiFlash_SendByte((SectorAddr & 0xFF00) >> 8);
	xBSP_SpiFlash_SendByte(SectorAddr & 0xFF);
	CS_SpiFlash_HIGH;
	xBSP_SpiFlash_WaitForWriteEnd();
	SpiFlashBusyFlag -= 1;
}

/*
***************************************************************************************************************
@ Name   : xSYS_SpiFlash_EraseChip

@ Brief  : SPI Flash操作，擦除整片SPI Flash

@ Param  : NULL

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13
***************************************************************************************************************
*/
void xSYS_SpiFlash_EraseChip(void)
{
	SpiFlashBusyFlag += 1;
	xBSP_SpiFlash_WriteEnable();

	CS_SpiFlash_LOW;
	xBSP_SpiFlash_SendByte(sFLASH_CMD_CE);
	CS_SpiFlash_HIGH;

	xBSP_SpiFlash_WaitForWriteEnd();
	SpiFlashBusyFlag -= 1;
}

/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_WritePage

@ Brief  : SPI Flash操作，在一页(0~65535)内写入少于256个字节的数据

@ Param  : uint8_t* pBuff：数据指针
@          uint32_t writeAddr：开始写入的地址(24bit)
@          uint16_t numByteToWrite：要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13

***************************************************************************************************************
*/
void xBSP_SpiFlash_WritePage(uint8_t* pBuff, uint32_t writeAddr, uint16_t numByteToWrite)
{
	SpiFlashBusyFlag += 1;
    xBSP_SpiFlash_WriteEnable();
    CS_SpiFlash_LOW;
    xBSP_SpiFlash_SendByte(sFLASH_CMD_WRITE);
    xBSP_SpiFlash_SendByte((writeAddr & 0xFF0000) >> 16);
    xBSP_SpiFlash_SendByte((writeAddr & 0xFF00) >> 8);
    xBSP_SpiFlash_SendByte(writeAddr & 0xFF);

    while (numByteToWrite--)
    {
        xBSP_SpiFlash_SendByte(*pBuff);
        pBuff++;
    }

    CS_SpiFlash_HIGH;
    xBSP_SpiFlash_WaitForWriteEnd();
	SpiFlashBusyFlag -= 1;
}

/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_WritePage

@ Brief  : SPI Flash操作，读取指定字节长度数据

@ Param  : uint8_t* pBuff：数据指针
@          uint32_t writeAddr：地址
@          uint16_t numByteToWrite：数据长度

@ Return : NULL

@ Author : WZB

@  Date  : 2019 - 04 - 13

***************************************************************************************************************
*/
void xSYS_SpiFlash_ReadBuffer(uint8_t* pBuff, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	SpiFlashBusyFlag += 1;
    CS_SpiFlash_LOW;
    xBSP_SpiFlash_SendByte(sFLASH_CMD_READ);
    xBSP_SpiFlash_SendByte((ReadAddr & 0xFF0000) >> 16);
    xBSP_SpiFlash_SendByte((ReadAddr& 0xFF00) >> 8);
    xBSP_SpiFlash_SendByte(ReadAddr & 0xFF);

    while (NumByteToRead--)
    {
        *pBuff = xBSP_SpiFlash_SendByte(sFLASH_DUMMY_BYTE);
        pBuff++;
    }

    CS_SpiFlash_HIGH;
	SpiFlashBusyFlag -= 1;
}

/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_Read_ManufactDeviceID

@ Brief  : SPI Flash操作，读取芯片ID序号

@ Param  : NULL

@ Return : 读取到的ID序号

@ Author : WZB

@  Date  : 2019 - 04 - 13

***************************************************************************************************************
*/
uint16_t xBSP_SpiFlash_Read_ManufactDeviceID(void)
{
    uint16_t Temp = 0;
	SpiFlashBusyFlag += 1;
    CS_SpiFlash_LOW;
    xBSP_SpiFlash_SendByte(sFLASH_CMD_ManufactDeviceID);
    xBSP_SpiFlash_SendByte(0X00);
    xBSP_SpiFlash_SendByte(0X00);
    xBSP_SpiFlash_SendByte(0X00);
    Temp = xBSP_SpiFlash_SendByte(sFLASH_DUMMY_BYTE)<<8;
    Temp |= xBSP_SpiFlash_SendByte(sFLASH_DUMMY_BYTE);

    CS_SpiFlash_HIGH;
	SpiFlashBusyFlag -= 1;
    return Temp;
}


/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_Read_IdentificationID

@ Brief  : SPI Flash操作，读取芯片ID序号

@ Param  : NULL

@ Return : 读取到的ID序号

@ Author : WZB

@  Date  : 2019 - 04 - 13
***************************************************************************************************************
*/
uint32_t xBSP_SpiFlash_Read_IdentificationID(void)
{
	static uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
    CS_SpiFlash_LOW;
	xBSP_SpiFlash_SendByte(sFLASH_CMD_IdentificationID);
	Temp0 = xBSP_SpiFlash_SendByte(sFLASH_DUMMY_BYTE);
	Temp1 = xBSP_SpiFlash_SendByte(sFLASH_DUMMY_BYTE);
	Temp2 = xBSP_SpiFlash_SendByte(sFLASH_DUMMY_BYTE);
    CS_SpiFlash_HIGH;

	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}

/*
***************************************************************************************************************
@ Name   : xBSP_SpiFlash_Write_NoCheck

@ Brief  : SPI Flash操作，无检验写多字节数据

@ Param  : uint8_t* pBuff:数据指针
@          uint32_t writeAddr:开始写入的地址(24bit)
@          uint16_t numByteToWrite:要写入的字节数(最大65535)

@ Return : NONE

@ Author : WZB

@  Date  : 2019 - 04 - 13

@  Note  : 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
***************************************************************************************************************
*/
void xBSP_SpiFlash_Write_NoCheck(uint8_t* pBuff,uint32_t writeAddr,uint16_t numByteToWrite)   
{ 			 		 
	SpiFlashBusyFlag += 1;
	uint16_t pageremain;	   
	pageremain=256-writeAddr%256; //单页剩余的字节数		 	    
	if(numByteToWrite<=pageremain)pageremain=numByteToWrite;//不大于256个字节
	while(1)
	{	   
		xBSP_SpiFlash_WritePage(pBuff,writeAddr,pageremain);
		if(numByteToWrite==pageremain)
        {
            break;//写入结束了
        }
	 	else
		{
			pBuff+=pageremain;
			writeAddr+=pageremain;	

			numByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(numByteToWrite>256)
            {
                pageremain=256;                   //一次可以写入256个字节
            }
			else
            {
                pageremain=numByteToWrite; 	      //不够256个字节了
            }
		}
	}
	SpiFlashBusyFlag -= 1;
}

/*
***************************************************************************************************************
@ Name   : xSYS_SpiFlash_WriteBuffer

@ Brief  : 写SPI FLASH,在指定地址开始写入指定长度的数据,

@ Param  : pBuff:数据存储区
@          riteAddr:开始写入的地址(24bit)						
@          numByteToWrite:要写入的字节数(最大65535)   

@ Return : 是否正在操作SPI FLASH，0-无操作，非0-有操作

@ Author : WZB

@  Date  : 2019 - 04 - 15

@  Note  : 该函数带擦除操作！
@          该函数会将相应扇区4KByte数据读取出来，修改所要更新的内容后在写入SPI FLASH.
***************************************************************************************************************
*/
void xSYS_SpiFlash_WriteBuffer(uint8_t* pBuff,uint32_t writeAddr,uint16_t numByteToWrite)   
{
    if(xSYS_SpiFlash_CheckUse())  return;

	SpiFlashBusyFlag += 1;
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * psFlashBuff;	  
   	psFlashBuff=sFlashBuff;	     
 	secpos=writeAddr/4096;//扇区地址  
	secoff=writeAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;
 	if(numByteToWrite<=secremain)
    {
        secremain=numByteToWrite;
    }
    
	while(1) 
	{	
		xSYS_SpiFlash_ReadBuffer(psFlashBuff,secpos*4096,4096);
		xSYS_SpiFlash_EraseSector(secpos*4096);
		for(i=0;i<secremain;i++)
		{
			psFlashBuff[i+secoff]=pBuff[i];	  
		}
		xBSP_SpiFlash_Write_NoCheck(psFlashBuff,secpos*4096,4096);
        
		if(numByteToWrite==secremain)
        {
            break;
        }
		else
		{
			secpos++;
			secoff=0; 

		   	pBuff+=secremain;
			writeAddr+=secremain;	   
		   	numByteToWrite-=secremain;
			if(numByteToWrite>4096)
            {
                secremain=4096;
            }
			else 
            {
                secremain=numByteToWrite;
            }
		}
	}
	SpiFlashBusyFlag -= 1;
}

/*
***************************************************************************************************************
@ Name   : xSYS_SpiFlash_CheckUse

@ Brief  : 检查是否正在使用SPI FLASH

@ Param  : NULL

@ Return : 是否正在操作SPI FLASH，0-无操作，非0-有操作

@ Author : WZB

@  Date  : 2019 - 04 - 15
***************************************************************************************************************
*/
uint8_t xSYS_SpiFlash_CheckUse(void)
{
    return SpiFlashBusyFlag;
}

//--TEST--------------------------------------------
#define  FLASH_WriteAddress     (1024*1023)
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  countof(a)   (sizeof(a) / sizeof(*(a)))
#define  BufferSize   (countof(Tx_Buffer)-1)

uint8_t cmpBuff(uint8_t* buff1, uint8_t* buff2, uint32_t leng)
{
    uint32_t i=leng;

    for(i=0;i<leng;i++)
    {
        if(buff1[i]!=buff2[i])
        {
            return 1;//不相等
        }
    }

    return 0;
}

/*
***************************************************************************************************************
@ Name   : xSYS_SpiFlash_Test

@ Brief  : SPI Flash操作，读写测试

@ Param  : NULL

@ Return : 测试结果，0-读写成功，1-读写失败

@ Author : WZB

@  Date  : 2019 - 04 - 15

@  Note  : 注意不要擦除到有用区域
***************************************************************************************************************
*/
uint8_t xSYS_SpiFlash_Test(void)
{
	int i;
	uint8_t result = 0;
	uint8_t Rx_Buffer[256];
	uint8_t Tx_Buffer[256];

	for(i = 0;i < 255;i++)		Tx_Buffer[i] = i/2;
	xSYS_SpiFlash_EraseSector(FLASH_SectorToErase);
	xBSP_SpiFlash_WritePage(Tx_Buffer, FLASH_WriteAddress, BufferSize);
	xSYS_SpiFlash_ReadBuffer(Rx_Buffer, FLASH_ReadAddress, BufferSize); 

	if(cmpBuff((uint8_t*)&Tx_Buffer, (uint8_t*)&Tx_Buffer, 256))
    {
        //不相等
        result = 1;
    }

	xSYS_SpiFlash_EraseSector(FLASH_SectorToErase);
	
	xSYS_SpiFlash_ReadBuffer(Rx_Buffer, FLASH_ReadAddress, BufferSize); 

	return result;
} 

