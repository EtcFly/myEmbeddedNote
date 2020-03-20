/* ----------------------- AVR includes -------------------------------------*/
#include "stm32f1xx_hal.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_50US_TICKS           ( 450UL )

TIM_HandleTypeDef htim2;
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )//
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  __HAL_RCC_TIM2_CLK_ENABLE();
  
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = (50 * usTim1Timerout50us)  + 1; //72000000/720=1000000
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
     
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
     
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    
  }
  HAL_TIM_Base_Start_IT(&htim2);

  HAL_NVIC_SetPriority(TIM2_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  
  vMBPortTimersDisable();

    return TRUE;
}


void xMBPortTimersClose(void)
{
	vMBPortTimersDisable();
}

inline void
vMBPortTimersEnable(void)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	HAL_TIM_Base_Start_IT(&htim2);
}

inline void
vMBPortTimersDisable(void)
{
	HAL_TIM_Base_Stop_IT(&htim2);
}

void vMBPortTimersDelay(USHORT usTimeOutMS)
{
	
}

void TIM2_IRQHandler(void)
{
	if (__HAL_TIM_GET_IT_SOURCE(&htim2, TIM_FLAG_UPDATE))
	{
		__HAL_TIM_CLEAR_IT(&htim2, TIM_FLAG_UPDATE);
		(void)pxMBPortCBTimerExpired();
	}
}


