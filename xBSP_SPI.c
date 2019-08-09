/**************************************************START OF FILE*****************************************************/

/*------------------------------------------------------------------------------------------------------------------
Includes
*/
#include "stm32f2xx_hal.h"
#include "xBSP_SPI.h"

#if SPI_EN_CRITICAL
#include "FreeRTOS.h"
#include "task.h"
#endif
/**Variable declaration************************************************************************/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
extern uint8_t startRTOS;
extern uint8_t FootLedPowerOnFlag;//1-上电显示，0-正常显示模式

/*
***************************************************************************************************************
@ Name   : xBSP_SPI1_Init

@ Brief  : SPI初始化配置

@ Param  : SPI_HandleTypeDef *hspi
@          SPI_TypeDef* SPIx

@ Return : NONE

@ Author : WZB

@  Date  : 2019 - 03 - 27
***************************************************************************************************************
*/
void xBSP_SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;

	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		while(1);
	}
}

/*
***************************************************************************************************************
@ Name   : xBSP_SPI2_Init

@ Brief  : SPI初始化配置

@ Param  : SPI_HandleTypeDef *hspi
@          SPI_TypeDef* SPIx

@ Return : NONE

@ Author : WZB

@  Date  : 2019 - 04 - 24
***************************************************************************************************************
*/
void xBSP_SPI2_Init(void)
{
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;

	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		while(1);
	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hspi->Instance==SPI1)
    {
        __HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
	    __HAL_RCC_GPIOB_CLK_ENABLE();

        /**SPI1 GPIO Configuration    
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI 
        PA4     ------> SPI1_NSS  SPI1_CS_MAIN_UPDATE (SPI FLASH CS)
        PB0    ------>  SPI1_CS_C489_UPDATA (SPI_FLASH)
        */
		
		/* SPI SCK GPIO pin configuration  */
		GPIO_InitStruct.Pin       = GPIO_PIN_5;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* SPI MISO GPIO pin configuration  */
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Pin       = GPIO_PIN_6;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* SPI MOSI GPIO pin configuration  */
		GPIO_InitStruct.Pin       = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		
		//改路SPI的CS等相关引脚配置
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST; 

		GPIO_InitStruct.Pin = GPIO_PIN_4;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);//165PL & SPI FLASH CS
		GPIO_InitStruct.Pin = GPIO_PIN_0;	
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); //DSP_FLASH CS
		
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }

    if(hspi->Instance==SPI2)
    {
        __HAL_RCC_SPI2_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
	    __HAL_RCC_GPIOC_CLK_ENABLE();

        /**SPI2 GPIO Configuration    
        PB13     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI 
        PB12     ------> SPI2_NSS  MAIN_LED_SPI_CS/595/165CS
        PC5     ------> 165PL
        */
		
		/* SPI SCK GPIO pin configuration  */
		GPIO_InitStruct.Pin       = GPIO_PIN_13;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* SPI MISO GPIO pin configuration  */
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Pin       = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* SPI MOSI GPIO pin configuration  */
		GPIO_InitStruct.Pin       = GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		//改路SPI的CS等相关引脚配置
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST; 

		GPIO_InitStruct.Pin = GPIO_PIN_12;	
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); //595/165CS
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);//165PL
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    }
}


/*
***************************************************************************************************************
@ Name   : xBSP_SPI1_WriteReadByte

@ Brief  : SPI读写一个字节数据

@ Param  : uint8_t Byte：写入的数据

@ Return : 读取到的数据

@ Author : WZB

@  Date  : 2019 - 03 - 27
***************************************************************************************************************
*/
uint8_t xBSP_SPI1_WriteReadByte(uint8_t Byte)
{
	uint8_t receivedbyte = 0;
#if SPI_EN_CRITICAL
	if(startRTOS)
	if(!FootLedPowerOnFlag)
	{
		taskENTER_CRITICAL(); //进入临界区 
	}
#endif

	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &Byte, (uint8_t*) &receivedbyte, 1, 1000);

#if SPI_EN_CRITICAL
	if(startRTOS)
	if(!FootLedPowerOnFlag)
	{
		taskEXIT_CRITICAL(); //退出临界区
	}
#endif
	return receivedbyte;
}

/*
***************************************************************************************************************
@ Name   : xBSP_SPI2_WriteReadByte

@ Brief  : SPI读写一个字节数据

@ Param  : uint8_t Byte：写入的数据

@ Return : 读取到的数据

@ Author : WZB

@  Date  : 2019 - 04 - 24
***************************************************************************************************************
*/
uint8_t xBSP_SPI2_WriteReadByte(uint8_t Byte)
{
	uint8_t receivedbyte = 0;
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*) &Byte, (uint8_t*) &receivedbyte, 1, 1000);
	return receivedbyte;
}


/**************************************************END OF FILE**********************************************************/
