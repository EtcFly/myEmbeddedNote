/**
  ******************************************************************************
  * @file    netconf.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Network connection configuration
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"   

int32_t QEICode[4]={0,0,0,0};    
 
//#define		USE_QEI1
#define		USE_QEI2
//#define		USE_QEI3 

//#define		REMAP_QEI1
//#define		REMAP_QEI2
//#define		REMAP_QEI3
//#define		REMAP_QEI3
 
#ifdef	USE_QEI1 


	#define		RCC_PORT_QEI1		(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
	#define		RCC_QEI1_TIM		RCC_APB1Periph_TIM4
	#define		PORT_QEI1			GPIOB	
	#define		PIN_A_QEI1			GPIO_Pin_6
	#define		PIN_B_QEI1			GPIO_Pin_7
	#define		TIM_QEI1			TIM4 

#endif


#ifdef	USE_QEI2 
	#ifndef 	REMAP_QEI2
		#define		RCC_PORT_QEI2		(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
		#define		RCC_QEI2_TIM		RCC_APB1Periph_TIM3
		#define		PORT_QEI2			GPIOA	
		#define		PIN_A_QEI2			GPIO_Pin_6
		#define		PIN_B_QEI2			GPIO_Pin_7
		#define		TIM_QEI2			TIM3 
	#else
		//û����֤��
		#define		RCC_PORT_QEI2		(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO)
		#define		RCC_QEI2_TIM		RCC_APB1Periph_TIM3
		#define		PORT_QEI2			  GPIOC	
		#define		PIN_A_QEI2			GPIO_Pin_6
		#define		PIN_B_QEI2			GPIO_Pin_7
		#define		TIM_QEI2			  TIM3 
	#endif	 
#endif
 
#ifdef	USE_QEI3 
	#define		RCC_PORT_QEI3		(RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO)
	#define		RCC_QEI3_TIM		RCC_APB2Periph_TIM1
	#define		PORT_QEI3			GPIOE	
	#define		PIN_A_QEI3			GPIO_Pin_9
	#define		PIN_B_QEI3			GPIO_Pin_11
	#define		TIM_QEI3			TIM1 
#endif 

 
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitQEI
*	����˵��: ������������ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
void bsp_InitQEI(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
//	TIM_ICInitTypeDef 		TIM_ICInitStructure;
	  
#ifdef	USE_QEI1 	  
	RCC_APB2PeriphClockCmd(RCC_PORT_QEI1, ENABLE);  			//��GPIOʱ�ӣ�
	RCC_APB1PeriphClockCmd(RCC_QEI1_TIM, ENABLE);				//��TIM4��ʱ��
	
	GPIO_InitStructure.GPIO_Pin =  PIN_A_QEI1|PIN_B_QEI1;		//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(PORT_QEI1, &GPIO_InitStructure);
	
	#ifdef	REMAP_QEI1
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);			//��ӳ������
	#endif
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;						    //Ԥ��Ƶ��Ϊ1	 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//���������ϼ���ģʽ  
	TIM_TimeBaseStructure.TIM_Period =0xFFFF;					        //�Զ���װ�ؼĴ��� 		     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//ʱ�ӷ�Ƶ���� 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0; 
	TIM_TimeBaseInit(TIM_QEI1,&TIM_TimeBaseStructure);					//дTIMX���Ĵ�������
	 
//	TIM_ICStructInit(&TIM_ICInitStructure); 
// 	TIM_ICInitStructure.TIM_ICFilter = 6;	
// 	TIM_ICInit(TIM_QEI1,&TIM_ICInitStructure);
	    
	TIM_EncoderInterfaceConfig(TIM_QEI1,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_SetCounter(TIM_QEI1,0);
	TIM_Cmd(TIM_QEI1,ENABLE); 
#endif	


#ifdef	USE_QEI2 	 
	RCC_APB2PeriphClockCmd(RCC_PORT_QEI2, ENABLE);  			//��GPIOʱ�ӣ�
	RCC_APB1PeriphClockCmd(RCC_QEI2_TIM, ENABLE);				//��TIMx��ʱ��
	
	GPIO_InitStructure.GPIO_Pin =  PIN_A_QEI2|PIN_B_QEI2;		//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(PORT_QEI2, &GPIO_InitStructure);
	 
	#ifdef	REMAP_QEI2
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);			//��ӳ������
	#endif 
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;						    //Ԥ��Ƶ��Ϊ1	 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//���������ϼ���ģʽ  
	TIM_TimeBaseStructure.TIM_Period =0xFFFF;					        //�Զ���װ�ؼĴ��� 		     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//ʱ�ӷ�Ƶ���� 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0; 
	TIM_TimeBaseInit(TIM_QEI2,&TIM_TimeBaseStructure);					//дTIMX���Ĵ�������
	 
//	TIM_ICStructInit(&TIM_ICInitStructure); 
// 	TIM_ICInitStructure.TIM_ICFilter = 6;	
// 	TIM_ICInit(TIM_QEI1,&TIM_ICInitStructure);
	    
	TIM_EncoderInterfaceConfig(TIM_QEI2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_SetCounter(TIM_QEI2,0);
	TIM_Cmd(TIM_QEI2,ENABLE); 
#endif



#ifdef	USE_QEI3 	 
	RCC_APB2PeriphClockCmd(RCC_PORT_QEI3, ENABLE);  			//��GPIOʱ�ӣ�
	RCC_APB2PeriphClockCmd(RCC_QEI3_TIM, ENABLE);				//��TIMx��ʱ��
	
	GPIO_InitStructure.GPIO_Pin =  PIN_A_QEI3|PIN_B_QEI3;		//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(PORT_QEI3, &GPIO_InitStructure);
	 
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);			//��ӳ������ 
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;						    //Ԥ��Ƶ��Ϊ1	 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//���������ϼ���ģʽ  
	TIM_TimeBaseStructure.TIM_Period =0xFFFF;					        //�Զ���װ�ؼĴ��� 		     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//ʱ�ӷ�Ƶ���� 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0; 
	TIM_TimeBaseInit(TIM_QEI3,&TIM_TimeBaseStructure);					//дTIMX���Ĵ�������
	 
//	TIM_ICStructInit(&TIM_ICInitStructure); 
// 	TIM_ICInitStructure.TIM_ICFilter = 6;	
// 	TIM_ICInit(TIM_QEI1,&TIM_ICInitStructure);
	    
	TIM_EncoderInterfaceConfig(TIM_QEI3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_SetCounter(TIM_QEI3,0);
	TIM_Cmd(TIM_QEI3,ENABLE); 
#endif
	
}

 
/* 
*********************************************************************************************************
*	�� �� ��: bsp_GetQEI
*	����˵��: ��ȡ����������ֵ
*	��    ��: Ch:[0..3]ָ��0-3ͨ�� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t bsp_GetQEI(uint8_t Ch)
{  
	if(Ch>3)		//���ͨ����
	Ch=0;	 
	return QEICode[Ch];
}

/* 
*********************************************************************************************************
*	�� �� ��: bsp_QEI_Task
*	����˵��: ��Ҫ�����Ե�ִ�д˺��������ٵ�20mSһ�ε�ִ�С�
*	��    ��: �� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_QEI_Task(void)
{ 
	static int16_t CntL[4]={0,0,0,0}; 
	int16_t CntN[4];
	int16_t Temp; 
#ifdef	USE_QEI1
 	CntN[0]=TIM_QEI1->CNT;    
 	Temp=CntN[0]-CntL[0]; 
 	QEICode[0]+=Temp;
 	CntL[0]=CntN[0]; 
#endif	
#ifdef	USE_QEI2
 	CntN[1]=TIM_QEI2->CNT;   
	Temp=CntN[1]-CntL[1];    
	QEICode[1]+=Temp;
	CntL[1]=CntN[1];    
#endif	
#ifdef	USE_QEI3
 	CntN[2]=TIM_QEI3->CNT;
 	Temp=CntN[2]-CntL[2];    
	QEICode[2]+=Temp;
	CntL[2]=CntN[2];
#endif	
#ifdef	USE_QEI4
 	CntN[3]=TIM_QEI4->CNT;    
	Temp=CntN[3]-CntL[3];
	QEICode[3]+=Temp;
	CntL[3]=CntN[3];
#endif	 
}
  
  

/******************* (C) COPYRIGHT 2015 Xingao Tech *****END OF FILE****/
