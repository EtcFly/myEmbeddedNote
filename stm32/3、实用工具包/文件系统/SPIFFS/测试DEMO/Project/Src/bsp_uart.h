/****************************************Copyright (c)**************************************************
**                                �����θ߿Ƽ����޹�˾
**                                     
**                                
** ��   ��   ��: bsp_uart.h
** ����޸�����: 2015/6/23
** ��        ��: 
** ��	     ��: V1.1
** ��  ��  о Ƭ:  ����Ƶ��: 
** IDE: 
**********************************************************************************************************/
#ifndef __BSP_UART_H
#define	__BSP_UART_H

#include "stm32f1xx_hal.h"

#define		UART_ADDR				0x01			//���ڵ�ַ 

//������Щ������Ҫʹ�ã�ʹ�õ�����Ϊ1  
#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	0
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0
#define	UART6_FIFO_EN	0 

//#define	UART3_USE_RS485_EN	1	//����3����ΪRS485
 
/* ����˿ں� */
typedef enum
{
	COM1 = 0,		/* USART1  PA9, PA10 ��  PB6, PB7 */
	COM2 = 1,		/* USART2, PA2, PA3�� PD5,PD6	  */
	COM3 = 2,		/* USART3, PB10, PB11 			  */
	COM4 = 3,		/* UART4, PC10, PC11 			  */
	COM5 = 4,		/* UART5, PC12, PD2 			  */
	COM6 = 5		/* USART6, PC6, PC7 STM32F107�޴˴���*/
}COM_PORT_E;

 
/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			115200L
	#define UART1_TX_BUF_SIZE	1*500
	#define UART1_RX_BUF_SIZE	1*500
#endif
 
#if UART2_FIFO_EN == 1
	#define UART2_BAUD			115200L
	#define UART2_TX_BUF_SIZE	1*500
	#define UART2_RX_BUF_SIZE	1*500
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			115200L		
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			38400L
	#define UART4_TX_BUF_SIZE	1*500
	#define UART4_RX_BUF_SIZE	1*500
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			115200L
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif

#if UART6_FIFO_EN == 1
	#define UART6_BAUD			115200L
	#define UART6_TX_BUF_SIZE	1*1024
	#define UART6_RX_BUF_SIZE	1*1024
#endif
  
//�û�ʵ��ʹ��ֻ��Ҫ����������
void bsp_InitCOM(void);														//BSP���ڳ�ʼ��
uint8_t ComSend(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);		//COM�ڷ��ͺ��� 
uint8_t ComGet(COM_PORT_E _ucPort, uint8_t *_pByte);						//COM�ڽ��պ���
 

#endif




	

	

	












