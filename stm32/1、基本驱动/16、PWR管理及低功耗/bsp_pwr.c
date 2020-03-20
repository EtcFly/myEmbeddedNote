/**************************************************************************************************************
*@文 件 名: bsp_pwr.c
*@开发环境: STM32F103C8 + MDK5.10.0.0
*@文件功能: 低功耗相关功能的实现
*@版    本: V1.0.0
*@描    述:
**************************************************************************************************************
*=============================================================================================================
* 版本           时间                    作者                         说明
*-------------------------------------------------------------------------------------------------------------
* V1.0.0        180424                   wy                        创建文档
**************************************************************************************************************/
#include "stm32f10x.h"
#include "TypeConfig.h"
#include "bsp_pwr.h"


/**************************************************************************************************************
                                   Private   Define
**************************************************************************************************************/
/**
*@def
*bit4   SEVONPEND          RW            当发生异常悬起时请发送事件,用于在一个新的中断悬起时从
																					WFE指令处唤醒,不管这个中断的优先级是否比现在高,都唤醒
																					如果没有WFE导致睡眠,在下次使用WFE时将立即唤醒
*bit3   保留
*bit2   SLEEPDEEP          RW            当进入睡眠模式时,使能外部的SLEEPDEEP信号以允许停止系统时钟
*bit1   SLEEPONEXIT        RW            激活"sleepOnExit"功能
*bit0   保留
*/
#define   SEVONPEND_BIT                  (0X01<<4)
#define   SLEEPDEEP_BITS                 (0X01<<2)
#define   SLEEPONEXIT_BITS               (0X01<<1)
#define   SYSTEM_CR_REG                  (*(volatile uint32 *)(0XE000ED10)) //系统控制寄存器
	


/**************************************************************************************************************
*@函数名: static void pwr_nvicConfig(void)
*@功  能:实现低功耗唤醒相关的中断事件的优先级配置
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
static void pwr_nvicConfig(void)
{
	
}



/**************************************************************************************************************
*@函数名: void bsp_pwrInit(void)
*@功  能: 实现低功耗硬件的初始化
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
/*低功耗方式选择因素
1、最低电源消耗   2、最快启动时间   3、唤醒的条件
STM32F10X三种低功耗模式 
 1>睡眠模式(Cotex-M3内核停止,但所以外设包括内核核心外设NVIC,SysTick仍在运行)
 2>停止模式(所以时钟停止(外设时钟))
 3>待机模式(1.8V电源关闭(该电源用于提供系统内核以及内存的供电))
*/
void bsp_pwrInit(void)
{
//	__disable_irq();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_DeInit();
	
	pwr_nvicConfig();
//	PWR_BackupAccessCmd(ENABLE);
//	PWR_PVDLevelConfig(PWR_PVDLevel_2V2);
//	PWR_PVDCmd(ENABLE);
//	PWR_WakeUpPinCmd(ENABLE);
}



/**************************************************************************************************************
*@函数名: void pwr_enterSleep(enum SLEEP_TYPE sleepType, enum SLEEP_MODE sleepMode)
*@功  能: 实现睡眠模式
*@输  入: sleepType睡眠类型 可选立即睡眠还是中断完成睡眠SLEEP_MODE_NOW     SLEEP_MODE_ON_EXIT
*         sleepMode睡眠模式 是WFI还是WFE
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
//睡眠模式(仅关闭内核时钟) 唤醒时间最短  功耗最大
/*
进入:  WFI          和            WFE
唤醒: 任一中断                 唤醒事件
*/
//sleep_mode   SLEEP_MODE_NOW     SLEEP_MODE_ON_EXIT
void pwr_enterSleep(enum SLEEP_TYPE sleepType, enum SLEEP_MODE sleepMode)
{
	uint32 temp;
	temp = SYSTEM_CR_REG;
	
	temp &= ~SLEEPDEEP_BITS; //清除深度睡眠位
	if (sleepMode == SLEEP_TYPE_NOW)
	{
			temp &= ~SLEEPONEXIT_BITS;
	}
	else if (sleepMode == SLEEP_TYPE_ON_EXIT)
	{
			temp |= SLEEPONEXIT_BITS;		
	}
	else 
	{
		return ; //fail
	}
	
	SYSTEM_CR_REG = temp;
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //清除SysTick的运行标志
	
	if (sleepMode == SLEEP_MODE_WFI) 	__WFI(); //进入中断
	else 		__WFE();
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/**************************************************************************************************************
*@函数名: void pwr_enterStopMode(uint8_t PWR_STOPEntry)
*@功  能: 实现停止睡眠模式
*@输  入: PWR_STOPEntry_WFI或者PWR_STOPEntry_WFE
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@描  述: 在停止模式下 外部中断输入可以唤醒WFE和WFI  外部事件输入仅仅可以唤醒WFI  
**************************************************************************************************************/
//停机模式(关闭所有1.8V区域时钟, HSI和HSE振荡器关闭)
/*
进入:PDDS和LPDS位 + SLEEPDEEP位 + WFI或WFE
唤醒:任一外部中断(在外部中断寄存器设置) 
*/
void pwr_enterStopMode(uint8_t PWR_STOPEntry)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry);
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}



/**************************************************************************************************************
*@函数名:void pwr_enterStandbyMode(void)
*@功  能: 待机模式相关低功耗函数的实现
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
//待机模式(关闭所有1.8V区域时钟, HSI和HSE振荡器关闭, 电压调节器关闭)
/*
进入:PDDS位 + SLEEPDEEP位 + WFI或WFE
唤醒:WKUP引脚的上升沿、RTC闹钟事件、NRST引脚上的外部复位、IWDG复位
*/
void pwr_enterStandbyMode(void)
{
	PWR_EnterSTANDBYMode();
	PWR_ClearFlag(PWR_FLAG_SB | PWR_FLAG_WU); //清除待机位和WAKUP事件标志
}




/**************************************************************************************************************
*@函数名:void PVD_IRQHandler(void)
*@功  能: 电压阈值检测中断
*@输  入: 无
*@输  出: 无
*@返  回: 无
*@日  期: 180424
*@作  者: Wy
*@版  本: V1.0.0
*@修  改: None
**************************************************************************************************************/
//掉电紧急任务处理
void PVD_IRQHandler(void)
{
	if (PWR_GetFlagStatus(PWR_FLAG_WU) == SET) //唤醒中断
	{
	   PWR_ClearFlag(PWR_FLAG_WU);
	}
	
	if (PWR_GetFlagStatus(PWR_FLAG_SB) == SET) //开始进入待机中断
	{
	   PWR_ClearFlag(PWR_FLAG_SB);
	}
	
	if (PWR_GetFlagStatus(PWR_FLAG_PVDO) == SET) //低电压中断 用于紧急数据存储
	{
	   PWR_ClearFlag(PWR_FLAG_PVDO);
	}	
}






















