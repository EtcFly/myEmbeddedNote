/**************************************************************************************************************************
*@fileName: timer.h
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.4
*@describe: None
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190218                                           Create File
===========================================================================================================================
**************************************************************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H



typedef void (*callBack_Type)(void *p_arg);
typedef enum 
{
    TIMER_RUN_ALL,   //一直运行
    TIMER_STOP_ALL,  //一直停止
    TIMER_RUN_CTN,   //指定运行次数
}TIEMR_PERMI; //定时器运行权限



void timer_initAll(void);
void timer_schedRunning(void);
uint8_t timer_getUsage(void);
int8_t timer_taskInstall(uint8_t *pName, callBack_Type pFunc, void *p_arg, uint32_t tDelay,
                         uint32_t offest_delay, uint8_t prio, TIEMR_PERMI runMode, uint32_t runCount);
int8_t timer_taskUnInstall(callBack_Type func_name);
void timer_debug_showAllUseTimer(void);

#endif
