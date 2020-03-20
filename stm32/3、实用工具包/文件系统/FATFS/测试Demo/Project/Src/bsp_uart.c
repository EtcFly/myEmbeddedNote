/**********************************************************************************************************
 *@fileName: bsp_uart.c
 *@platform: stm32l4xx + MDK5.26.2.0
 *@version:  v2.0.1
 *@describe: usart driver interface
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v2.0.0         151109         ArmFly          Create  file
  v2.0.1         190530         EtcFly          Migrate to stm32l4xx platform and used hal library      
**********************************************************************************************************/
#include "stm32f1xx_hal.h" 
#include "bsp_uart.h"

/* �����豸�ṹ�� */
typedef struct
{
	UART_HandleTypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	__IO uint8_t *pTxBuf;			/* ���ͻ����� */
	__IO uint8_t *pRxBuf;			/* ���ջ����� */
	__IO uint16_t usTxBufSize;		/* ���ͻ�������С */
	__IO uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;		/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;			/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;		/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;		/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;			/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;		/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(void);	/* �����յ����ݵĻص�����ָ�� */
}UART_T;  
 

/* ����ÿ�����ڽṹ����� */
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */
	UART_HandleTypeDef husart1;
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
	UART_HandleTypeDef husart2;
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART4_FIFO_EN == 1
	static UART_T g_tUart4;
	static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART5_FIFO_EN == 1
	static UART_T g_tUart5;
	static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART6_FIFO_EN == 1
	static UART_T g_tUart6;
	static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* ���ջ����� */
#endif

/* 485���ߺ궨�� */
#if UART3_USE_RS485_EN == 1
	#define RS485_DIR_GPIO_CLK              RCC_APB2Periph_GPIOB
	#define	RS485_DIR_GPIO_PORT				GPIOB
	#define RS485_DIR_GPIO_PIN				GPIO_Pin_12
	
	#define RS485_TX_EN()					GPIO_SetBits(RS485_DIR_GPIO_PORT,RS485_DIR_GPIO_PIN)
	#define RS485_RX_EN()					GPIO_ResetBits(RS485_DIR_GPIO_PORT,RS485_DIR_GPIO_PIN)
	
	void USART3_SendBeforCallBack( void );
	void USART3_SendOverCallBack( void );
	
#endif

UART_T *ComToUart(COM_PORT_E _ucPort);

/*
*********************************************************************************************************
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void UartVarInit(void)
{
#if UART1_FIFO_EN == 1
	g_tUart1.uart = &husart1;						/* STM32 �����豸 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart1.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart1.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = &husart2;						/* STM32 �����豸 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
	g_tUart2.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ�� */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart2.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usTxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usRxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart2.usTxCount = 0;						/* �����͵����ݸ��� */	
	g_tUart2.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart2.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart2.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = USART3;						/* STM32 �����豸 */
	g_tUart3.pTxBuf = g_TxBuf3;					/* ���ͻ�����ָ�� */
	g_tUart3.pRxBuf = g_RxBuf3;					/* ���ջ�����ָ�� */
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart3.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart3.usTxRead = 0;						/* ����FIFO������ */
	g_tUart3.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart3.usRxRead = 0;						/* ����FIFO������ */
	g_tUart3.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart3.usTxCount = 0;						/* �����͵����ݸ��� */	
	g_tUart3.SendBefor = USART3_SendBeforCallBack;/* ��������ǰ�Ļص����� */
	g_tUart3.SendOver = USART3_SendOverCallBack;/* ������Ϻ�Ļص����� */
	g_tUart3.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART4_FIFO_EN == 1
	g_tUart4.uart = UART4;						/* STM32 �����豸 */
	g_tUart4.pTxBuf = g_TxBuf4;					/* ���ͻ�����ָ�� */
	g_tUart4.pRxBuf = g_RxBuf4;					/* ���ջ�����ָ�� */
	g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart4.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart4.usTxRead = 0;						/* ����FIFO������ */
	g_tUart4.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart4.usRxRead = 0;						/* ����FIFO������ */
	g_tUart4.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart4.usTxCount = 0;						/* �����͵����ݸ��� */	
	g_tUart4.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart4.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart4.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART5_FIFO_EN == 1
	g_tUart5.uart = UART5;						/* STM32 �����豸 */
	g_tUart5.pTxBuf = g_TxBuf5;					/* ���ͻ�����ָ�� */
	g_tUart5.pRxBuf = g_RxBuf5;					/* ���ջ�����ָ�� */
	g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart5.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart5.usTxRead = 0;						/* ����FIFO������ */
	g_tUart5.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart5.usRxRead = 0;						/* ����FIFO������ */
	g_tUart5.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart5.usTxCount = 0;						/* �����͵����ݸ��� */	
	g_tUart5.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart5.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart5.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART6_FIFO_EN == 1
	g_tUart6.uart = USART6;						/* STM32 �����豸 */
	g_tUart6.pTxBuf = g_TxBuf6;					/* ���ͻ�����ָ�� */
	g_tUart6.pRxBuf = g_RxBuf6;					/* ���ջ�����ָ�� */
	g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart6.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart6.usTxRead = 0;						/* ����FIFO������ */
	g_tUart6.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart6.usRxRead = 0;						/* ����FIFO������ */
	g_tUart6.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart6.usTxCount = 0;						/* �����͵����ݸ��� */	
	g_tUart6.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart6.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart6.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif
}


/*
*********************************************************************************************************
*	�� �� ��: UartHardInit
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void UartHardInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
#if UART1_FIFO_EN == 1		/* ����1 TX = PA9   RX = PA10 �� TX = PB6   RX = PB7*/

	/* ��1���� ����GPIO */
	#if 1	
		/* �� UART ʱ�� */
		__HAL_RCC_USART1_CLK_ENABLE();
		
		#if 0 //choice PA9/PA10
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		/**USART1 GPIO Configuration    
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX 
		*/
		/* �� GPIO ʱ�� */
			__HAL_RCC_GPIOA_CLK_ENABLE();
			GPIO_InitStructure.Pin = GPIO_PIN_9|GPIO_PIN_10;	
		#else
			__HAL_RCC_GPIOB_CLK_ENABLE();
			GPIO_InitStructure.Pin = GPIO_PIN_6|GPIO_PIN_7;	
		#endif
		
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	#endif

	/* ��2���� ���ô���Ӳ������ */
	  husart1.Instance = USART1;
	  husart1.Init.BaudRate = UART1_BAUD;
	  husart1.Init.WordLength = UART_WORDLENGTH_8B;
	  husart1.Init.StopBits = UART_STOPBITS_1;
	  husart1.Init.Parity = UART_PARITY_NONE;
	  husart1.Init.Mode = UART_MODE_TX_RX;
	  husart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  husart1.Init.OverSampling = UART_OVERSAMPLING_16;
	  if (HAL_UART_Init(&husart1) != HAL_OK)
	  {
		 return ;
	  }
	  
	  __HAL_UART_ENABLE_IT(&husart1, UART_IT_RXNE); 
	  while (__HAL_UART_GET_FLAG(&husart1, UART_FLAG_TC) != RESET)
	  {
		__HAL_UART_CLEAR_FLAG(&husart1, UART_FLAG_TC);
	  }

#endif

#if UART2_FIFO_EN == 1		/* ����2 TX = PA2   RX = PA3 �� TX = PD5   RX = PD6*/

	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_UART_DeInit(&husart2);
	/* ��1���� ����GPIO */

	/* �� GPIO ʱ�� */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* �� UART ʱ�� */
	__HAL_RCC_USART2_CLK_ENABLE();

    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	husart2.Instance = USART2;
	husart2.Init.BaudRate = UART2_BAUD;
	husart2.Init.WordLength = UART_WORDLENGTH_8B;
	husart2.Init.StopBits = UART_STOPBITS_1;
	husart2.Init.Parity = UART_PARITY_NONE;
	husart2.Init.Mode = UART_MODE_TX_RX;
	husart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	husart2.Init.OverSampling = UART_OVERSAMPLING_16;
	husart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	husart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&husart2) != HAL_OK)
	{

	}
	__HAL_UART_ENABLE_IT(&husart2, UART_IT_RXNE);
	while (__HAL_UART_GET_FLAG(&husart2, UART_FLAG_TC) != RESET)
	{
	__HAL_UART_CLEAR_IT(&husart2, UART_CLEAR_TCF);
	}
#endif
}
  
/*
*********************************************************************************************************
*	�� �� ��: UartNVICConfig
*	����˵��: ���ô���Ӳ���ж�.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartNVICConfig(void)  
{
	/* Configure the NVIC Preemption Priority Bits */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	#if UART1_FIFO_EN == 1		 
	/* USART1 interrupt Init */
	HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	#endif
	
	#if UART2_FIFO_EN == 1		 
	/* ʹ�ܴ���1�ж� */
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 2);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
	#endif
	
	#if UART3_FIFO_EN == 1		 
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	#endif
	 
	#if UART4_FIFO_EN == 1		 
	/* ʹ�ܴ���4�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	#endif 
} 
  		
/*
*********************************************************************************************************
*	�� �� ��: bsp_COMInit
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��:  	��
*	�� �� ֵ: 	��
*********************************************************************************************************
*/		
void bsp_InitCOM(void)
{
	UartVarInit();
	UartHardInit();
	UartNVICConfig();
}				

/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: uartָ��
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_tUart3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return &g_tUart4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return &g_tUart5;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM6)
	{
		#if UART6_FIFO_EN == 1
			return &g_tUart6;
		#else
			return 0;
		#endif
	}
	else
	{
		/* �����κδ��� */
		return 0;
	}
}  

 
///*
//*********************************************************************************************************
//*	�� �� ��: UartIsTxBuffFull
//*	����˵��: �жϷ��ͻ����Ƿ�����
//*	��    ��:  	_pUart : �����豸
//*	�� �� ֵ: 	1��ʾ���ͻ���������0��ʾ���ͻ���δ��
//*********************************************************************************************************
//*/	
//uint8_t UartIsTxBuffFull(UART_T *_pUart)
//{ 
//	uint16_t ucCount; 
//	USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
//	ucCount=_pUart->usTxCount;
//	USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
//	
//	if(ucCount==_pUart->usTxBufSize)
//	return 1;
//	else
//	return 0;		
//}
//
//
///*
//*********************************************************************************************************
//*	�� �� ��: UartIsRxBuffFull
//*	����˵��: �жϽ��ջ����Ƿ�����
//*	��    ��:  	_pUart : �����豸
//*	�� �� ֵ: 	1��ʾ���ջ���������0��ʾ���ջ���δ��
//*********************************************************************************************************
//*/	
//uint8_t UartIsRxBuffFull(UART_T *_pUart)
//{
//	uint16_t ucCount; 
//	USART_ITConfig(_pUart->uart, USART_IT_RXNE, DISABLE);
//	ucCount=_pUart->usRxCount;
//	USART_ITConfig(_pUart->uart, USART_IT_RXNE, ENABLE);	
//	if(ucCount>=_pUart->usRxBufSize)
//	{
//		return 1;
//	}	  
//	else
//	return 0;	
//}

/*
*********************************************************************************************************
*	�� �� ��: UartPutChar
*	����˵��: ͨ��Uart�ڷ���һ���ֽ����ݣ�������д�뻺�棬Ȼ���پ����Ƿ���
*	��    ��:  	COM_PORT_E : COM�ţ�_ucDat�����͵����ݡ�
*	�� �� ֵ: 	1��ʾ���ͻ���������0��ʾ���ͻ���δ��
*********************************************************************************************************
*/
static uint8_t UartPutChar(UART_T *_pUart, uint8_t _ucDat)
{
	__HAL_UART_DISABLE_IT(_pUart->uart, UART_IT_TC);				//��ֹ���ڷ��ͽ����ж�
	//���ͻ����������淢�͵����ݺ��Ե�
	if(_pUart->usTxCount==_pUart->usTxBufSize)
	{
		__HAL_UART_ENABLE_IT(_pUart->uart, UART_IT_TC);			//�򿪴��ڷ��ͽ����ж�  
		return 1;
	}	
	
	_pUart->pTxBuf[_pUart->usTxWrite]=_ucDat;		//���ͻ�����д����
	_pUart->usTxWrite++;
	if(_pUart->usTxWrite>=_pUart->usTxBufSize)
	_pUart->usTxWrite=0;	
	  
	_pUart->usTxCount++; 
	if(_pUart->usTxCount==1)
	{
		if(!(_pUart->usTxWrite))
		{
			_pUart->usTxRead=_pUart->usTxBufSize-1;
		}	
		else
		_pUart->usTxRead=_pUart->usTxWrite-1;
		
		HAL_UART_Transmit(_pUart->uart, (uint8_t *)&_pUart->pTxBuf[_pUart->usTxRead], 1, 100);		   
		_pUart->usTxRead=_pUart->usTxWrite;
	}   
	
	__HAL_UART_ENABLE_IT(_pUart->uart, UART_IT_TC);			//�򿪴��ڷ��ͽ����ж�   
	return 0;
}

 


/*
*********************************************************************************************************
*	�� �� ��: UartGetChar
*	����˵��: �Ӵ��ڽ��ջ�������ȡ1�ֽ����� 
*	��    ��: _pUart : �����豸
*			  _pByte : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: 0 ��ʾ������  1��ʾ��ȡ������
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{  
	__HAL_UART_DISABLE_IT(_pUart->uart, UART_IT_RXNE);	
	if(_pUart->usRxCount)		//��ʾ���ջ�����������
	{ 
		_pUart->usRxCount--;
		
		*_pByte=_pUart->pRxBuf[_pUart->usRxRead];
		 
		_pUart->usRxRead++;
		if(_pUart->usRxRead>=_pUart->usRxBufSize)
		_pUart->usRxRead=0;	
		  
		__HAL_UART_ENABLE_IT(_pUart->uart, UART_IT_RXNE);	
		return 1;
	}	
	else
	{ 
		__HAL_UART_ENABLE_IT(_pUart->uart, UART_IT_RXNE);	
		return 0;
	} 
}

///*
//*********************************************************************************************************
//*	�� �� ��: ComClearRxFifo
//*	����˵��: ���㴮�ڽ��ջ�����
//*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
//*	�� �� ֵ: ��
//*********************************************************************************************************
//*/
//void ComClearRxFifo(COM_PORT_E _ucPort)
//{
//	UART_T *pUart; 
//	pUart = ComToUart(_ucPort);
//	if (pUart == 0)
//	{
//		return;
//	} 
//	pUart->usRxWrite = 0;
//	pUart->usRxRead = 0;
//	pUart->usRxCount = 0;
//}
//
//
///*
//*********************************************************************************************************
//*	�� �� ��: ComClearTxFifo
//*	����˵��: ���㴮�ڷ��ͻ�����
//*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
//*	�� �� ֵ: ��
//*********************************************************************************************************
//*/
//void ComClearTxFifo(COM_PORT_E _ucPort)
//{
//	UART_T *pUart;
//	pUart = ComToUart(_ucPort);
//	if (pUart == 0)
//	{
//		return;
//	} 
//	pUart->usTxWrite = 0;
//	pUart->usTxRead = 0;
//	pUart->usTxCount = 0;
//}


/*
*********************************************************************************************************
*	�� �� ��: ComSend
*	����˵��: ��COM�ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��:  	COM_PORT_E : COM�ţ�_ucaBuf�����͵�����ָ�룬_usLen�������ݳ���
*	�� �� ֵ: 	1��ʾ���ͻ���������0��ʾ���ͻ���δ��,����0xFF��ʾ�����ڵĴ���
*********************************************************************************************************
*/	
uint8_t ComSend(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;
	uint8_t _ucFull;
	uint16_t i;
	
	pUart=ComToUart(_ucPort);
	if (pUart == 0)
	{		
		return 0xFF;
	}

	for(i=0;i<_usLen;i++)
	{
		_ucFull=UartPutChar(pUart,_ucaBuf[i]);
		if(_ucFull)
		{
			return 1;
		}
	} 
		
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: comGetChar
*	����˵��: �Ӵ��ڻ�������ȡ1�ֽڣ��������������������ݾ���������
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
*********************************************************************************************************
*/
uint8_t ComGet(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;
	uint8_t Exist;
	pUart=ComToUart(_ucPort);
	Exist=UartGetChar(pUart,_pByte);
	return Exist;
}


/*
*********************************************************************************************************
*	�� �� ��: UartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{	
//	if(__HAL_UART_GET_FLAG(_pUart->uart, UART_FLAG_RXNE) != RESET)	//���ͽ����ж�
//	{
//		//�ڻ���������״̬�£��������յ�������
//		if (_pUart->usRxCount < _pUart->usRxBufSize)
//		{
//			/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
//			if (HAL_UART_Receive(_pUart->uart, (uint8_t *)&_pUart->pRxBuf[_pUart->usRxWrite], 1, 100) == HAL_OK) {
//				_pUart->usRxCount++;
//				_pUart->usRxWrite++;
//				if(_pUart->usRxWrite>=_pUart->usRxBufSize)
//				_pUart->usRxWrite=0; 
//			}
//		} 
//		else
//		{
//			uint8_t data;
//			HAL_UART_Receive(_pUart->uart, &data, 1, 100);
//		}	
//	}	 
//	if (__HAL_UART_GET_FLAG(_pUart->uart, UART_FLAG_TC) != RESET)	//��������ж�
//	{
//		_pUart->usTxCount--;
//		if(_pUart->usTxCount)		//������з��͵�����
//		{
//			if (HAL_UART_Transmit(_pUart->uart, (uint8_t *)&_pUart->pTxBuf[_pUart->usTxRead], 1, 100) == HAL_OK) {
//				_pUart->usTxRead++;
//				if(_pUart->usTxRead>=_pUart->usTxBufSize)
//				_pUart->usTxRead=0; 	
//			}
//		}	
//		else
//		{
//			__HAL_UART_CLEAR_FLAG(_pUart->uart,UART_FLAG_TC);			 //���������ɱ�־
//		}
//	} 

	if(__HAL_UART_GET_FLAG(_pUart->uart, UART_FLAG_RXNE) != RESET)	//���ͽ����ж�
	{
		//�ڻ���������״̬�£��������յ�������
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
			if (HAL_UART_Receive(_pUart->uart, (uint8_t *)&_pUart->pRxBuf[_pUart->usRxWrite], 1, 100) == HAL_OK) {
				_pUart->usRxCount++;
				_pUart->usRxWrite++;
				if(_pUart->usRxWrite>=_pUart->usRxBufSize)
				_pUart->usRxWrite=0; 
			}
			else
			{
				__HAL_UART_CLEAR_FLAG(_pUart->uart, UART_FLAG_RXNE);
			}
		} 
		else
		{
			uint8_t data;
			if (HAL_UART_Receive(_pUart->uart, &data, 1, 100) != HAL_OK)
			{
				__HAL_UART_CLEAR_FLAG(_pUart->uart, UART_FLAG_RXNE);
			}
		}
		return ;
	}	 
	if (__HAL_UART_GET_FLAG(_pUart->uart, UART_FLAG_TC) != RESET)	//��������ж�
	{
		_pUart->usTxCount--;
		if(_pUart->usTxCount)		//������з��͵�����
		{
			if (HAL_UART_Transmit(_pUart->uart, (uint8_t *)&_pUart->pTxBuf[_pUart->usTxRead], 1, 100) == HAL_OK) {
				_pUart->usTxRead++;
				if(_pUart->usTxRead>=_pUart->usTxBufSize)
				_pUart->usTxRead=0; 	
			}
			else
			{
				__HAL_UART_CLEAR_FLAG(_pUart->uart,UART_FLAG_TC);			 //���������ɱ�־
			}
		}	
		else
		{
			__HAL_UART_CLEAR_FLAG(_pUart->uart,UART_FLAG_TC);			 //���������ɱ�־
		}
		return ;
	} 
//	__HAL_UART_CLEAR_FLAG(_pUart->uart, UART_CLEAR_PEF | UART_CLEAR_FEF | UART_CLEAR_NEF  \
//	                                  | UART_CLEAR_OREF | UART_CLEAR_IDLEF |  UART_CLEAR_TCF
//                                      | UART_CLEAR_LBDF	| UART_CLEAR_CTSF | UART_CLEAR_CMF | UART_CLEAR_WUF);
}


/*
*********************************************************************************************************
*	�� �� ��: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}
#endif

//#include "includes.h"

#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
//	OSIntEnter();
	UartIRQ(&g_tUart2);
//	OSIntExit();
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
	UartIRQ(&g_tUart3);
}
#endif

#if UART4_FIFO_EN == 1
void UART4_IRQHandler(void)
{
	UartIRQ(&g_tUart4);
}
#endif

#if UART5_FIFO_EN == 1
void UART5_IRQHandler(void)
{
	UartIRQ(&g_tUart5);
}
#endif

#if UART6_FIFO_EN == 1
void USART6_IRQHandler(void)
{
	UartIRQ(&g_tUart6);
}
#endif

/*------------------------------------------------------------
 * Function Name  : USART3_SendBeforCallBack
 * Description    : ���ڷ���ǰ�ص�����
 * Input          : None
 * Output         : None
 * Return         : None
 *------------------------------------------------------------*/
void USART3_SendBeforCallBack( void )
{
	#ifdef	UART3_USE_RS485_EN
		RS485_TX_EN();
	#endif
}

/*------------------------------------------------------------
 * Function Name  : USART3_SendOverCallBack
 * Description    : ���ڷ�����ɻص�����
 * Input          : None
 * Output         : None
 * Return         : None
 *------------------------------------------------------------*/
void USART3_SendOverCallBack( void )
{
	#ifdef	UART3_USE_RS485_EN
		RS485_RX_EN();
	#endif
}

#include "stdio.h"
/*
*********************************************************************************************************
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//int fputc(int ch, FILE *f)
//{
//	#if 1	/* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
//		ComSend(COM1,(uint8_t *)&ch,1);
//		return ch;
//	#else	/* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */
//		/* дһ���ֽڵ�USART1 */
//		USART_SendData(USART2, (uint8_t) ch);
//		/* �ȴ����ͽ��� */
//		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
//		{
//			
//		}
//		return ch;
//	#endif
//}

    
  
/******************* (C) COPYRIGHT 2015 XinGao Tech ********END OF FILE***************************/  
