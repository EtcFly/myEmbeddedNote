/**************************************************************************************************************
*@File:      bsp_dog.c
*@IDE :      MDK5.10.0.0+STM32F103ZET6
*@Function:  ʵ��WWDG��IWDG������
*@Author:    Wy
*@Date:      180412
*@Version:   V1.0.0
*@brief:     ��
***************************************************************************************************************/
#include "stm32f10x.h"
#include "bsp_dog.h"





/*********************************************************************************************************************
*@FuncName: static void wwdog_nvicConfig(void)
*@brief:    ʵ�ֿ��Ź��ж����ȼ�����
*@parm:     None
*@retval:   None
*@Author:   Wy
*@Date:     180412
*@modif:    None
*********************************************************************************************************************/
#if WWDOG_ENABLE && WWDOG_IT_ENABLE
static void wwdog_nvicConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}
#endif

/*********************************************************************************************************************
*@FuncName: void bsp_dogInit(void)
*@brief:    ʵ�ֿ��Ź��ĳ�ʼ��
*@parm:     None
*@retval:   None
*@Author:   Wy
*@Date:     180412
*@modif:    None
*********************************************************************************************************************/
void bsp_dogInit(void)
{
#if WWDOG_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_DeInit();
/*
Twwdg = Tpclk1*4096*2^WDGB*(T[5:0]+1)
WDGTB       ��С��ʱֵ      ���ʱֵ
	0			113us          7.28ms
	1           227us          14.56ms
	2           455us          29.12ms
	3           910us          58.25ms
*/
	WWDG_SetPrescaler(WWDG_Prescaler_1);
	if ((uint32_t)WWDOG_PRIEOD > 58250)
	{
		while (1);
	}
#define  WWDOG_LOAD_VALUE     ((uint8_t)((WWDOG_PRIEOD)/113))	//113��ʾ�÷ֱ����µ���С��ʱֵ
	WWDG_SetCounter(WWDOG_LOAD_VALUE+0X40); //�������40H
	WWDG_Enable(WWDOG_LOAD_VALUE+0X40);
	
#if WWDOG_IT_ENABLE
	wwdog_nvicConfig();
	WWDG_EnableIT();
#endif //WWDOG_IT_ENABLE
	
#endif
	
#if IWDOG_ENABLE
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	if ((uint32_t)IWDOG_PRIEOD > 262144)
	{
		while (1);
	}
/*
��Ƶ     ��Сʱ��     ���ʱ��(ms)
 4          0.1         409.6
 8          0.2         819.2
16          0.4         1638.4
32          0.8         3276.8
64          1.6         6553.6
128         3.2         13107.2
256         6.4         26214.4
*/
	while (IWDG_GetFlagStatus(IWDG_FLAG_PVU));
	IWDG_SetPrescaler(IWDG_Prescaler_4);
	
	while (IWDG_GetFlagStatus(IWDG_FLAG_RVU));	
	IWDG_SetReload(IWDOG_PRIEOD); //0.1��4��Ƶ����С�ֱ���0.1ms 
	IWDG_ReloadCounter();
	IWDG_Enable();
#endif
}

#if WWDOG_ENABLE
#if WWDOG_IT_ENABLE //���ڴ��ڿ��Ź���ʵ��

/*********************************************************************************************************************
*@FuncName: static void System_DataSave(void)
*@brief:    ϵͳ�������ݱ��溯��
*@parm:     None
*@retval:   None
*@Author:   Wy
*@Date:     180412
*@modif:    None
*********************************************************************************************************************/
static void System_DataSave(void)
{
	
}
//���Ź��ж�-ʵ���ж�ι��
void WWDG_IRQHandler(void)
{
	if (WWDG_GetFlagStatus()&0x01)
	{
		WWDG_ClearFlag();
		WWDG_SetCounter(WWDOG_LOAD_VALUE+0X40);//��Ҫ�����ݱ���
		System_DataSave();
		WWDG_SetCounter(0X3F);// ��λ
	}
}
#endif

/*********************************************************************************************************************
*@FuncName: void bsp_wwdogFeed(void)
*@brief:    ���ڿ��Ź�ι��
*@parm:     None
*@retval:   None
*@Author:   Wy
*@Date:     180412
*@modif:    None
*********************************************************************************************************************/
void bsp_wwdogFeed(void)
{
	WWDG_SetCounter(WWDOG_LOAD_VALUE+0X40);
}
#endif //WWDOG_ENABLE

#if IWDOG_ENABLE  //���ڶ������Ź���ʵ��

/*********************************************************************************************************************
*@FuncName: void bsp_iwdogFeed(void)
*@brief:    �������Ź�ι��
*@parm:     None
*@retval:   None
*@Author:   Wy
*@Date:     180412
*@modif:    None
*********************************************************************************************************************/
void bsp_iwdogFeed(void)
{
	uint8_t t = 0xff;
	while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) && t--);	
	IWDG_ReloadCounter();
}
#endif



/********************************Copyright (C)2015 - 2025 YM Inc. All Rights Reserved***********************************/





