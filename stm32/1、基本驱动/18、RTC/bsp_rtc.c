
#include "bsp_rtc.h"







static void bsp_rtc_clock_config(void)
{
  RCC_BackupResetCmd(DISABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);
  
  PWR_BackupAccessCmd(ENABLE);
  RCC_LSICmd(ENABLE);
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  RCC_RTCCLKCmd(ENABLE);
}

void bsp_rtcInit(void)
{
 //闹钟中断配置
  EXTI_InitTypeDef EXTI_InitStrucut;
  EXTI_InitStrucut.EXTI_Line = EXTI_Line17;
  EXTI_InitStrucut.EXTI_LineCmd = ENABLE;
  EXTI_InitStrucut.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStrucut.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStrucut);
 
//RTC配置
  bsp_rtc_clock_config(); 
  RTC_WaitForSynchro();  //等待同步

  RTC_WaitForLastTask(); //等待上一次写完成
  RTC_SetPrescaler(7971*SYS_NORM_POWER_LED_FLASH_TIM); //写入时钟分频
  RTC_WaitForLastTask(); //等待上一次写完成	
  RTC_SetCounter(0x00);
  RTC_WaitForLastTask(); //等待上一次写完成
  RTC_SetAlarm(RTC_GetCounter()+2); //2表示设置计数值增长2时触发闹钟中断
  RTC_WaitForLastTask();	
		
  RTC_ClearFlag(RTC_FLAG_ALR | RTC_FLAG_SEC | RTC_FLAG_OW);
  RTC_ITConfig(RTC_IT_SEC | RTC_IT_ALR, ENABLE);
}


void RTCAlarm_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_ALR) == SET)
  {
     RTC_ClearITPendingBit(RTC_IT_ALR);
	 //需要重新配置RTC闹钟计数值
  }
  EXTI_ClearITPendingBit(EXTI_Line17);
}

void RTC_IRQHandler(void)
{
   static  uint8_t t;
   if (RTC_GetITStatus(RTC_IT_SEC) == SET)
   {
	  t++;
   }
   RTC_ClearITPendingBit(RTC_IT_SEC);
}


