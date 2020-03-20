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
		//没有验证过
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
*	函 数 名: bsp_InitQEI
*	功能说明: 正交编码器初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/  
void bsp_InitQEI(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
//	TIM_ICInitTypeDef 		TIM_ICInitStructure;
	  
#ifdef	USE_QEI1 	  
	RCC_APB2PeriphClockCmd(RCC_PORT_QEI1, ENABLE);  			//打开GPIO时钟，
	RCC_APB1PeriphClockCmd(RCC_QEI1_TIM, ENABLE);				//打开TIM4的时钟
	
	GPIO_InitStructure.GPIO_Pin =  PIN_A_QEI1|PIN_B_QEI1;		//初始化GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(PORT_QEI1, &GPIO_InitStructure);
	
	#ifdef	REMAP_QEI1
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);			//重映射引脚
	#endif
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;						    //预分频器为1	 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//计数器向上计数模式  
	TIM_TimeBaseStructure.TIM_Period =0xFFFF;					        //自动重装载寄存器 		     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//时钟分频因子 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0; 
	TIM_TimeBaseInit(TIM_QEI1,&TIM_TimeBaseStructure);					//写TIMX各寄存器参数
	 
//	TIM_ICStructInit(&TIM_ICInitStructure); 
// 	TIM_ICInitStructure.TIM_ICFilter = 6;	
// 	TIM_ICInit(TIM_QEI1,&TIM_ICInitStructure);
	    
	TIM_EncoderInterfaceConfig(TIM_QEI1,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_SetCounter(TIM_QEI1,0);
	TIM_Cmd(TIM_QEI1,ENABLE); 
#endif	


#ifdef	USE_QEI2 	 
	RCC_APB2PeriphClockCmd(RCC_PORT_QEI2, ENABLE);  			//打开GPIO时钟，
	RCC_APB1PeriphClockCmd(RCC_QEI2_TIM, ENABLE);				//打开TIMx的时钟
	
	GPIO_InitStructure.GPIO_Pin =  PIN_A_QEI2|PIN_B_QEI2;		//初始化GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(PORT_QEI2, &GPIO_InitStructure);
	 
	#ifdef	REMAP_QEI2
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);			//重映射引脚
	#endif 
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;						    //预分频器为1	 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//计数器向上计数模式  
	TIM_TimeBaseStructure.TIM_Period =0xFFFF;					        //自动重装载寄存器 		     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//时钟分频因子 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0; 
	TIM_TimeBaseInit(TIM_QEI2,&TIM_TimeBaseStructure);					//写TIMX各寄存器参数
	 
//	TIM_ICStructInit(&TIM_ICInitStructure); 
// 	TIM_ICInitStructure.TIM_ICFilter = 6;	
// 	TIM_ICInit(TIM_QEI1,&TIM_ICInitStructure);
	    
	TIM_EncoderInterfaceConfig(TIM_QEI2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_SetCounter(TIM_QEI2,0);
	TIM_Cmd(TIM_QEI2,ENABLE); 
#endif



#ifdef	USE_QEI3 	 
	RCC_APB2PeriphClockCmd(RCC_PORT_QEI3, ENABLE);  			//打开GPIO时钟，
	RCC_APB2PeriphClockCmd(RCC_QEI3_TIM, ENABLE);				//打开TIMx的时钟
	
	GPIO_InitStructure.GPIO_Pin =  PIN_A_QEI3|PIN_B_QEI3;		//初始化GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(PORT_QEI3, &GPIO_InitStructure);
	 
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);			//重映射引脚 
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;						    //预分频器为1	 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//计数器向上计数模式  
	TIM_TimeBaseStructure.TIM_Period =0xFFFF;					        //自动重装载寄存器 		     
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//时钟分频因子 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0; 
	TIM_TimeBaseInit(TIM_QEI3,&TIM_TimeBaseStructure);					//写TIMX各寄存器参数
	 
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
*	函 数 名: bsp_GetQEI
*	功能说明: 读取正交编码器值
*	形    参: Ch:[0..3]指定0-3通道 
*	返 回 值: 无
*********************************************************************************************************
*/
int32_t bsp_GetQEI(uint8_t Ch)
{  
	if(Ch>3)		//检查通道号
	Ch=0;	 
	return QEICode[Ch];
}

/* 
*********************************************************************************************************
*	函 数 名: bsp_QEI_Task
*	功能说明: 需要周期性的执行此函数，至少得20mS一次的执行。
*	形    参: 无 
*	返 回 值: 无
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
