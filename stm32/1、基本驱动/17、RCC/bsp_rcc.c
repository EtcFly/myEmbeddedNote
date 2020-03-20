/**************************************************************************************************************
*@文 件 名: bsp_rcc.c
*@开发环境: STM32F103C8 + MDK5.10.0.0
*@文件功能: 实现rcc时钟相关配置
*@版    本: V1.0.0
*@描    述:
**************************************************************************************************************
*=============================================================================================================
* 版本           时间                    作者                         说明
*-------------------------------------------------------------------------------------------------------------
*V1.0.0          180424                   wy                         创建文档
**************************************************************************************************************/

#include "TypeConfig.h"
#include "stm32f10x_rcc.h"
#include "bsp_rcc.h"





/**************************************************************************************************************
*@函数名:void RCC_MainClockSet(uint8_t clock)
*@功  能: 设置HSI或者HSE使能 
*@输  入: clock  0不是HSE使能  1 HSI使能
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void RCC_MainClockSet(uint8_t clock)
{
	if (clock == 0) //选择外部HSE时钟
	{
		RCC_HSEConfig(RCC_HSE_ON);
		RCC_HSICmd(DISABLE);
	}
	else //选择内部HSI时钟
	{
		RCC_HSEConfig(RCC_HSE_OFF);
		RCC_HSICmd(ENABLE);		
	}
}

/**************************************************************************************************************
*@函数名: void RCC_SysclkSet(uint8_t clock)
*@功  能: 设置SYSCLK时钟
*@输  入: clock 0不是HSI==SYSCLK   1HSE==SYSCLK   2 PLLCLK == SYSCLK 
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void RCC_SysclkSet(uint8_t clock)
{
	switch (clock)
	{
		case 0://HSI 8MHz时钟
		{
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); 
			RCC_PLLCmd(DISABLE); 
			break;
		}
		case 1: //HSE 8MHz时钟
		{
		  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
			RCC_PLLCmd(DISABLE);  
			break;
		}
		case 2: //PLLCLK作为SYSCLK 时钟输入
		{
			//RCC_PLLSource_HSI_Div2  RCC_PLLSource_HSE_Div1两种时钟选择
			RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_2); 
			RCC_PLLCmd(ENABLE); 
			break;
		}
	}
}

/**************************************************************************************************************
*@函数名: void RCC_AHBClockSet(void)
*@功  能: 设置AHB的时钟
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@描  述: AHB可设置2到512分屏
**************************************************************************************************************/
static void RCC_AHBClockSet(void)
{
   RCC_HCLKConfig(RCC_SYSCLK_Div512);
}

/**************************************************************************************************************
*@函数名: void RCC_APB1ClcokSet(void)
*@功  能: 设置APB1时钟分频
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void RCC_APB1ClcokSet(void)
{
   RCC_PCLK1Config(RCC_HCLK_Div1); //设置AHB到APB1的分频系数  max36MHz
}

/**************************************************************************************************************
*@函数名: void RCC_APB2ClockSet(void)
*@功  能: 设置APB2时钟分频
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void RCC_APB2ClockSet(void)
{
	 RCC_PCLK2Config(RCC_HCLK_Div16); //设置AHB到APB2的分屏系数 max72MHz
}

/**************************************************************************************************************
*@函数名: void RCC_ClockOutput(uint8_t clock)
*@功  能: 设置MCO引脚时钟输出源
*@输  入: clock 时钟源标号 
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void RCC_ClockOutput(uint8_t clock)
{
	 switch (clock)
	 {
		 case 0: //无时钟输出
	 	 { 
				RCC_MCOConfig(RCC_MCO_NoClock); break;
		 }
		 case 1: //SYSCLK输出
		 {
				RCC_MCOConfig(RCC_MCO_SYSCLK); break;		 
		 }
		 case 2: //HSI输出
		 {
		 		RCC_MCOConfig(RCC_MCO_HSI);	 break;
		 }
		 case 3://HSE输出
		 {
		 		RCC_MCOConfig(RCC_MCO_HSE);	break;	 	 
		 }
		 case 4: //PLLCLK/2输出
		 {
		 		RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);	break;			 
		 }
	 }
}

/**************************************************************************************************************
*@函数名: void RCC_SetRTClock(uint08 clock)
*@功  能: 设置RTC时钟源
*@输  入: clock时钟源标号 0LSE==RTC  1LSI==RTC   2 HSE/128==RTC
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void RCC_SetRTClock(uint08 clock)
{
	switch (clock)
	{
		 case 0: //LSE作为RTC时钟
		 {
			 //RCC_LSE_OFF RCC_LSE_Bypass
				RCC_LSEConfig(RCC_LSE_ON); 
			  RCC_LSICmd(DISABLE);
			  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); break;
		 }
		 case 1: //LSI作为RTC时钟
		 {
			 RCC_LSEConfig(RCC_LSE_Bypass);
			 RCC_LSICmd(ENABLE);
		   RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); break;
		 }
		 case 2: //以HSE的128分屏作为RTC时钟
		 {
			 RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128); break;
		 }
	}		
}

/**************************************************************************************************************
*@函数名: void RCC_USBClockSet(void)
*@功  能: USB时钟源配置
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@描  述: USB的时钟必须配置为48MHz 
**************************************************************************************************************/
static void RCC_USBClockSet(void)
{
	//usb时钟 48MHz
   RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
}


/**************************************************************************************************************
*@函数名: void RCC_ClockSet(void)
*@功  能: 设置整个系统的时钟
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180425
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
void RCC_ClockSet(void)
{
	RCC_MainClockSet(1); //HSI时钟
	RCC_SysclkSet(0); //HSI作为SYSCLK
	RCC_AHBClockSet();
	RCC_APB1ClcokSet();	
	RCC_APB2ClockSet();				
	RCC_ClockOutput(0); //无时钟输出	
	RCC_LSEConfig(RCC_LSE_Bypass);
	RCC_LSICmd(DISABLE);	
}


