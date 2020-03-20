/**********************************************************************************************************
 *@fileName: dm9051_port.c
 *@platform: 
 *@version:  v1.0.0
 *@describe: DM9051NP Network Card Transplantation of interface
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         1901126        EtcFly          Create  file
**********************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stm32l4xx_hal.h"
#include "dm9051_port.h"
#include "usart.h"


#define GPIO_DM9051_CS							GPIOD
#define GPIO_Pin_DM9051_CS						GPIO_PIN_12

#define SPI_DM9051_CS_LOW()         HAL_GPIO_WritePin(GPIO_DM9051_CS, GPIO_Pin_DM9051_CS, GPIO_PIN_RESET)
#define SPI_DM9051_CS_HIGH()        HAL_GPIO_WritePin(GPIO_DM9051_CS, GPIO_Pin_DM9051_CS, GPIO_PIN_SET)

//#define SPI_MOSI_H                  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
//#define SPI_MOSI_L                  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)

//#define SPI_MISO                    HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)

//#define SPI_CLK_H                   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)
//#define SPI_CLK_L                   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)


extern SPI_HandleTypeDef hspi2;
void dm_enter_critical(void)
{
	
}

void dm_exit_critical(void)
{
	
}

void spi_set_cs_high(void)
{
	SPI_DM9051_CS_HIGH();	
}

void spi_set_cs_low(void)
{
	SPI_DM9051_CS_LOW();
}

void delayUs(uint32_t us)
{
	volatile uint32_t t;
	for (t = 0; t < us; t++)
	{
		;
	}
}

int8_t spi_rw_data(uint8_t sendData, uint8_t *recvData)
{
	uint8_t recv = 0x00;
	HAL_StatusTypeDef status;
	while (!__HAL_SPI_GET_FLAG(&hspi2, SPI_FLAG_TXE));
//	*(volatile uint8_t *)(&hspi2.Instance->DR) = sendData;
//	
//	SET_BIT(hspi2.Instance->CR2, SPI_RXFIFO_THRESHOLD);
//	while (!__HAL_SPI_GET_FLAG(&hspi2, SPI_FLAG_RXNE));
//	*recvData = *(volatile uint8_t *)(&hspi2.Instance->DR);
	
	if ((status = HAL_SPI_TransmitReceive(&hspi2, &sendData, &recv, sizeof(uint8_t), 10000)) != HAL_OK)
	{
		if ((status = HAL_SPI_TransmitReceive(&hspi2, &sendData, &recv, sizeof(uint8_t), 10000)) != HAL_OK)
		{
			//dm9051_port.info_printf("try again , but also is fail timeout!\r\n");
			return -1;
		}
	}
	
	*recvData = recv;
//	SPI_CLK_H;
//	for (uint8_t i = 0; i < 8; i++)
//	{
//		if (sendData & 0x80) 
//		{
//			SPI_MOSI_H;
//		}
//		else 
//		{
//			SPI_MOSI_L;
//		}
//		SPI_CLK_L;
//		recv <<= 1;
//		if (SPI_MISO)	
//		{
//			recv |= 0x01;
//		}
//		sendData <<= 1;
//		SPI_CLK_H;
//	}

//	*recvData = recv;
	return  0;		
}

void delay(uint32_t t)
{
	HAL_Delay(t);
}


uint8_t buf[128];
int show_info(const char *info, ...)
{
	static uint8_t index = 1;
	va_list list;
	
	if (index == 0)
	{
		return -1;
	}
	
	index = 0;
	va_start(list, info);
	vsprintf((char *)buf, info, list);
	va_end(list);
	
	uint8_t len = strlen((void *)buf);
	while (HAL_UART_Transmit(&huart1, (void *)&buf, len, 1000) != HAL_OK);
	index = 1;
	return 0;
}

DM_Port_t  dm9051_port = 
{
	.enter_critical = dm_enter_critical,
	.exit_critical = dm_exit_critical,
	.chip_cs_active = spi_set_cs_low,
	.chip_cs_inactive = spi_set_cs_high,
	.spi_rw = spi_rw_data,
	.delay_ms = delay,
//	.info_printf = show_info,
};











