#include "stm32f1xx_hal.h"
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"



extern UART_HandleTypeDef huart2;

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	static uint16_t timeout = 0xff;
	
	if (xRxEnable) {
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	} else {
		__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
	}
//	
	if (xTxEnable) {
	//	__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
		timeout  = 0xff;
		while (!pxMBFrameCBTransmitterEmpty() && timeout--)
		{
			
		}
	} else {
	//	__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
	}
}

void xMBPortSerialClose( void )
{
	vMBPortSerialEnable(FALSE, FALSE);
	__HAL_UART_DISABLE(&huart2);
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  switch (ucPORT)
  {
	  case 1: huart2.Instance = USART1; break;
	  case 2: huart2.Instance = USART2; break;
	  case 3: huart2.Instance = USART3; break;	
	  default : return FALSE;
	  
  }
  
  huart2.Init.BaudRate = ulBaudRate;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  
  switch (eParity)
  {
	  case MB_PAR_NONE: huart2.Init.WordLength = UART_WORDLENGTH_8B;
						huart2.Init.Parity = UART_PARITY_NONE; break;
	  case MB_PAR_ODD:  huart2.Init.WordLength = UART_WORDLENGTH_9B; 
						huart2.Init.Parity = UART_PARITY_ODD; break;
	  case MB_PAR_EVEN: huart2.Init.WordLength = UART_WORDLENGTH_9B; 
						huart2.Init.Parity = UART_PARITY_EVEN; break;
  }

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    
  }
   __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC);

   vMBPortSerialEnable( TRUE, FALSE );
   HAL_NVIC_SetPriority(USART2_IRQn, 1, 2);
   HAL_NVIC_EnableIRQ(USART2_IRQn);
  
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
	if (HAL_UART_Transmit(&huart2, (UCHAR *)&ucByte, 1, 10) != HAL_OK)
	{
		return FALSE;
	}
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
	if (HAL_UART_Receive(&huart2, (UCHAR *)pucByte, 1, 10) != HAL_OK)
	{
		return FALSE;
	}
    return TRUE;
}

void USART2_IRQHandler(void)
{	
	if (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE)) {
		pxMBFrameCBByteReceived();
		__HAL_UART_CLEAR_PEFLAG(&huart2);
	}
}




