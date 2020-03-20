
#include "bsp_uart.h"
 
/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
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
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
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
	g_tUart1.uart = USART1;						/* STM32 �����豸 */
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
	g_tUart2.uart = USART2;						/* STM32 �����豸 */
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
	USART_InitTypeDef USART_InitStructure;
	
#if UART1_FIFO_EN == 1		/* ����1 TX = PA9   RX = PA10 �� TX = PB6   RX = PB7*/

	USART_DeInit(USART1);
	/* ��1���� ����GPIO */
	#if 1	/* TX = PA9   RX = PA10 */
		/* �� GPIO ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

		/* �� UART ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#else	/* TX = PB6   RX = PB7  */
		/* �� GPIO ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
		/* �� UART ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		 
		GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);		//ѡ��USART1��Remap����

		/* Configure USART1 Tx (PB.06) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure); 
		/* Configure USART1 Rx (PB.07) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure); 
	#endif

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	  
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */ 
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */    
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET)
	{
		;
	}
	USART_ClearFlag(USART1, USART_FLAG_TC);
#endif

#if UART2_FIFO_EN == 1		/* ����2 TX = PA2   RX = PA3 �� TX = PD5   RX = PD6*/

	USART_DeInit(USART2);
	/* ��1���� ����GPIO */
	#if 0	/* TX = PA2   RX = PA3 */
		/* �� GPIO ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

		/* �� UART ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
 
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#else	/* TX = PD5   RX = PD6  */
		/* �� GPIO ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
		/* �� UART ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		 
		GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);		//ѡ��USART2��Remap����

		/* Configure USART1 Tx (PB.06) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOD, &GPIO_InitStructure); 
		/* Configure USART1 Rx (PB.07) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOD, &GPIO_InitStructure); 
	#endif

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART2_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	  
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */ 
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */ 
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET)
	{
		;
	}
	USART_ClearFlag(USART2, USART_FLAG_TC);
#endif
	
#if UART3_FIFO_EN == 1		/* ����1 TX = PB10   RX = PB11 */ 

	USART_DeInit(USART3);
	/* ��1���� ����GPIO */
	#if 1	/* TX = PB10   RX = PB11 */
		/* �� GPIO ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
		
		#if UART3_USE_RS485_EN == 1
			RCC_APB1PeriphClockCmd(RS485_DIR_GPIO_CLK, ENABLE);
		#endif

		/* �� UART ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
 
		/* Configure USART1 Tx (PB.10) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure); 
		/* Configure USART1 Rx (PB.11) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		#if UART3_USE_RS485_EN == 1
			GPIO_InitStructure.GPIO_Pin = RS485_DIR_GPIO_PIN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(RS485_DIR_GPIO_PORT, &GPIO_InitStructure);
			
			RS485_TX_EN();
		#endif
		
	#else	/* TX = PB6   RX = PB7  */
		/* �� GPIO ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
		/* �� UART ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		 
		GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);		//ѡ��USART1��Remap����

		/* Configure USART1 Tx (PB.06) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure); 
		/* Configure USART1 Rx (PB.07) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure); 
	#endif

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART3_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	  
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */ 
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART3, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */ 
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET)
	{
		;
	}
	USART_ClearFlag(USART3, USART_FLAG_TC);
#endif
	
#if UART4_FIFO_EN == 1		/* ����4 TX = PC10   RX = PC11 */

	USART_DeInit(UART4);
	/* ��1���� ����GPIO */
	#if 1	/* TX = PC10   RX = PC11 */
		/* �� GPIO ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);

		/* �� UART ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		 
 
		/* Configure UART4 Tx (PC.10) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure); 
		/* Configure UART4 Rx (PC.11) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure); 
	#endif

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART4_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	  
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */ 
	/*
		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(UART4, ENABLE);		/* ʹ�ܴ���4 */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(UART4, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */ 
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET)
	{
		;
	}
	USART_ClearFlag(UART4, USART_FLAG_TC);
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
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	#if UART1_FIFO_EN == 1		 
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	#endif
	
	#if UART2_FIFO_EN == 1		 
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
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
	USART_ITConfig(_pUart->uart, USART_IT_TC,DISABLE);				//��ֹ���ڷ��ͽ����ж�
	//���ͻ����������淢�͵����ݺ��Ե�
	if(_pUart->usTxCount==_pUart->usTxBufSize)
	{
		USART_ITConfig(_pUart->uart, USART_IT_TC,ENABLE);			//�򿪴��ڷ��ͽ����ж�  
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
		
		USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);		   
		_pUart->usTxRead=_pUart->usTxWrite;
	}   
	
	USART_ITConfig(_pUart->uart, USART_IT_TC,ENABLE);			//�򿪴��ڷ��ͽ����ж�   
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
	USART_ITConfig(_pUart->uart, USART_IT_RXNE, DISABLE);  
	if(_pUart->usRxCount)		//��ʾ���ջ�����������
	{ 
		_pUart->usRxCount--;
		
		*_pByte=_pUart->pRxBuf[_pUart->usRxRead];
		 
		_pUart->usRxRead++;
		if(_pUart->usRxRead>=_pUart->usRxBufSize)
		_pUart->usRxRead=0;	
		  
		USART_ITConfig(_pUart->uart, USART_IT_RXNE, ENABLE);
		return 1;
	}	
	else
	{ 
		USART_ITConfig(_pUart->uart, USART_IT_RXNE, ENABLE);
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
	if(USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)	//���ͽ����ж�
	{
		//�ڻ���������״̬�£��������յ�������
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
			/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
			_pUart->pRxBuf[_pUart->usRxWrite]=USART_ReceiveData(_pUart->uart);
			_pUart->usRxWrite++;
			if(_pUart->usRxWrite>=_pUart->usRxBufSize)
			_pUart->usRxWrite=0; 
		} 
		else
		{
			USART_ReceiveData(_pUart->uart);
		}	
	}	 
	if (USART_GetITStatus(_pUart->uart, USART_IT_TC) != RESET)	//��������ж�
	{
		_pUart->usTxCount--;
		if(_pUart->usTxCount)		//������з��͵�����
		{
			USART_SendData(_pUart->uart,_pUart->pTxBuf[_pUart->usTxRead]);     
			_pUart->usTxRead++;
			if(_pUart->usTxRead>=_pUart->usTxBufSize)
			_pUart->usTxRead=0; 	
		}	
		else
		{
			USART_ClearFlag(_pUart->uart,USART_FLAG_TC);			 //���������ɱ�־
		}
	} 
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
int fputc(int ch, FILE *f)
{
	#if 1	/* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
		ComSend(COM1,(uint8_t *)&ch,1);
		return ch;
	#else	/* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */
		/* дһ���ֽڵ�USART1 */
		USART_SendData(USART2, (uint8_t) ch);
		/* �ȴ����ͽ��� */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
		{
			
		}
		return ch;
	#endif
}

    
  
/******************* (C) COPYRIGHT 2015 XinGao Tech ********END OF FILE***************************/  
