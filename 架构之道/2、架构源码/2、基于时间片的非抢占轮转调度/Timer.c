/**************************************************************************************************************************
*@fileName: Timer.C
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.4
*@describe: scheduler  timer
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190216                                           Create File
* V1.0.1      Wy     190321                                     将原来的数组形式的调度修改为链表形式、增加了定时器调用权限、
                                                                同时添加了timer_taskInstall、timer_taskUnInstall等函数 
* V1.0.2      Wy     190322                                     增加运行基准平移单元,避免任务运行时相互干扰导致抖动
                                                                增加统计任务(计算任务的调度数、定时器资源的使用比例等)
                                                                添加调试函数(打印任务链表, 任务信息)
* V1.0.3      Wy     190322                                     添加回调函数参数支持，支持指定次数运行模式    
* V1.0.4      Wy     190419                                     增加任务优先级设置, 不支持同等优先级的轮转调度(0 higest prio)
                                                                支持低功耗模式
===========================================================================================================================
**************************************************************************************************************************/
#include "stm32f10x.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"

/************************************************Private config**********************************************************************/
#define TIMER_COUNT 9          //定时器的数量
#define TIMER_RUN_COUNT_STAT 1 //是否开启统计任务 (1: Support  0: don't Support)
#define TIMER_DEBUG 1          //是否开启调试支持 (1: Support  0: don't Support)

#define TIMER_SUPPORT_ROBIN 0 //支持时间片轮转调度
#define MAX_TIMER_WHEEL 12    //最大时间轮辐 (default:12)

#define TIMER_PRINTF(args, ...) printf(args, ##__VA_ARGS__)

/************************************************Private typedef**********************************************************************/
#if TIMER_SUPPORT_ROBIN
typedef struct
{
   //  struct Timer_TypeDef *prev;
   struct Timer_TypeDef *next;
   uint8_t timer_ctn; //在本轮盘上的任务数量
} WHELL_ROBIN_TypeDef;

typedef struct
{
   WHELL_ROBIN_TypeDef wheel[MAX_TIMER_WHEEL];
} WHEEL_TypeDef; //同等优先级任务链表结构
#endif

typedef struct Timer_TypeDef
{
   uint32_t start;
   uint32_t tdelay;
   uint32_t offest_delay;
   uint8_t b_fristRun; //1:第一次运行   0:非第一次运行

   uint8_t *name; //describe information
   TIEMR_PERMI permi_run;
   callBack_Type pFunc;
   void *p_arg;

   uint8_t remain_run; //剩余运行次数(仅在mode为TIMER_RUN_CTN下有效)

   struct Timer_TypeDef *next;
   struct Timer_TypeDef *prev;

   uint8_t prio; //任务优先级
   uint8_t x;    //加速单元
   uint8_t y;

#if (TIMER_SUPPORT_ROBIN == 1u)
   WHEEL_TypeDef *wheel; //同等优先级任务
//   uint8_t wheel_taskCtn; //同优先级的总任务数
#endif

#if TIMER_RUN_COUNT_STAT
   uint32_t run_count;
#endif
} Timer_TypeDef;

typedef struct
{
   uint8_t max_timer;   //总共的定时器资源数
   uint8_t use_timer;   //已经使用的定时器
   uint8_t usage_timer; //定时器使用情况
} TimerUsage_TypeDef;

/************************************************Private Define**********************************************************************/
#define TIMER_VERSION "1.0.4" //V1.0.4

/************************************************static variable**********************************************************************/
static Timer_TypeDef sys_timer[TIMER_COUNT];
static Timer_TypeDef *headTimer, *freeTimer;         //链表的头节点
static Timer_TypeDef *timer_tcb[TIMER_COUNT];        //优先级控制块
static uint8_t readyTabl[(TIMER_COUNT - 1) / 8 + 1]; //就绪表
static uint8_t readyGrp;

#if TIMER_RUN_COUNT_STAT
static TimerUsage_TypeDef timer_usage; //定时器使用比例
#endif

#if TIMER_SUPPORT_ROBIN
WHEEL_TypeDef timer_wheel[TIMER_COUNT];
#endif

static uint8_t const OSUnMapTbl[256] = {
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x00 to 0x0F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x10 to 0x1F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x20 to 0x2F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x30 to 0x3F                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x40 to 0x4F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x50 to 0x5F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x60 to 0x6F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x70 to 0x7F                   */
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x80 to 0x8F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x90 to 0x9F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xA0 to 0xAF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xB0 to 0xBF                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xC0 to 0xCF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xD0 to 0xDF                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xE0 to 0xEF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  /* 0xF0 to 0xFF                   */
};

/***************************************************************************************************************************
*@funcName: uint32_t get_currSysCount(void)
*@describe: 获取系统心跳
*@input:    None
*@return:   当前系统的时间戳
****************************************************************************************************************************/
static __INLINE uint32_t get_currSysCount(void)
{
   extern uint32_t get_systemCount(void);
   return get_systemCount();
}

/***************************************************************************************************************************
*@funcName: static void timer_restart(Timer_TypeDef *timer)
*@describe: 定时器重装载
*@input:
      @tiemr: 需要重装载的定时器
*@return:  None
****************************************************************************************************************************/
static void timer_restart(Timer_TypeDef *timer)
{
   timer->start = get_currSysCount();
}

/***************************************************************************************************************************
*@funcName: void timer_init_set(Timer_TypeDef *timer, uint32_t tDelay)
*@describe: 定时器的初始化
*@input:
      @timer: 需要初始化的定时器
      @tDelay: 定时周期
*@return: None
****************************************************************************************************************************/
static void timer_init_set(Timer_TypeDef *timer, uint32_t tDelay)
{
   timer_restart(timer);
   timer->tdelay = tDelay;
}

/***************************************************************************************************************************
*@funcName: int8_t Is_timer_ready(Timer_TypeDef *timer)
*@describe: 检测定时周期是否到达
*@input:
      @timer: 检测的定时器
*@return:  0:未到达  其他:ready
****************************************************************************************************************************/
static int8_t is_timer_ready(Timer_TypeDef *timer)
{
   int8_t retV;
   uint32_t timDelay;
   if (timer->b_fristRun)
   {
      timDelay = timer->tdelay + timer->offest_delay;
   }
   else
   {
      timDelay = timer->tdelay;
   }
   retV = (int8_t)(get_currSysCount() - timer->start >= timDelay);
   if (retV)
   {
      timer_restart(timer);
      timer->b_fristRun = 0;
   }
   return retV;
}

/***************************************************************************************************************************
*@funcName: static void timer_clearReadyTabl(uint8_t prio)
*@describe: 清除指定任务的就绪表
*@input:
     @prio: 设置任务的优先级
*@return: None
****************************************************************************************************************************/
static void timer_clearReadyTabl(uint8_t prio)
{
   readyTabl[timer_tcb[prio]->x] &= ~(0x01 << timer_tcb[prio]->y);
   if (readyTabl[timer_tcb[prio]->x] == 0x00)
   {
      readyGrp &= ~(0x01 << timer_tcb[prio]->x);
   }
}

/***************************************************************************************************************************
*@funcName: static void timer_setReadyTabl(uint8_t prio)
*@describe: 设置指定任务的就绪表
*@input:
     @prio: 设置任务的优先级
*@return: None
****************************************************************************************************************************/
static void timer_setReadyTabl(uint8_t prio)
{
   readyGrp |= (0x01 << timer_tcb[prio]->x);
   readyTabl[timer_tcb[prio]->x] |= (0x01 << timer_tcb[prio]->y);
}

/***************************************************************************************************************************
*@funcName: static void timer_insertTimeWheel(Timer_TypeDef *timer)
*@describe: 向时间轮辐中插入一个定时器
*@input:
     @insertTimer: 本次需要插入的定时器
     @fristTimer:如果原来同等优先级任务只有1个,那么该参数应该指向该定时器, 否则应该设置为0
*@return: None
****************************************************************************************************************************/
#if TIMER_SUPPORT_ROBIN
static void timer_insertTimeWheel(Timer_TypeDef *insertTimer, Timer_TypeDef *fristTimer)
{

}
#endif
/***************************************************************************************************************************
*@funcName: static void timer_taskExecute(void)
*@describe: 对就绪的任务进行执行
*@input: None
*@return: None
****************************************************************************************************************************/
static void timer_taskExecute(void)
{
   uint8_t prio, x;
   Timer_TypeDef *pTimer_currentRun;

   if (readyGrp != 0X00)
   {
      while (readyGrp != 0x00)
      {
         x = OSUnMapTbl[readyGrp];
         prio = (x << 3) + OSUnMapTbl[readyTabl[x]];
         if (timer_tcb[prio] != 0x00)
         {
            pTimer_currentRun = timer_tcb[prio];
            pTimer_currentRun->pFunc(timer_tcb[prio]->p_arg);
#if TIMER_RUN_COUNT_STAT
            pTimer_currentRun->run_count++;
#endif  
         }
         timer_clearReadyTabl(prio);
      }
      pTimer_currentRun = (Timer_TypeDef *)0;
   }
   else
   {
      //system sleep
   }
}
/***************************************************************************************************************************
*@funcName: int8_t timer_taskInstall(uint8_t *pName, callBack_Type pFunc, uint32_t tDelay)
*@describe: 向定时器安装一个任务
*@input:
     @pName: 任务名
     @pFunc: 任务回调函数指针
     @tDelay: 任务调用延时
*@return:
      @retV:   0-success   -1 -fail
****************************************************************************************************************************/
int8_t
timer_taskInstall(uint8_t *pName, callBack_Type pFunc, void *p_arg, uint32_t tDelay,
                  uint32_t offest_delay, uint8_t prio, TIEMR_PERMI runMode, uint32_t runCount)
{
#if TIMER_RUN_COUNT_STAT
   uint8_t tmp = timer_usage.use_timer;
#endif

   Timer_TypeDef *phead1;
   if (((callBack_Type)0 == pFunc) || (0 == tDelay) || ((Timer_TypeDef *)0 == freeTimer) || (prio >= TIMER_COUNT)
#if TIMER_RUN_COUNT_STAT
       || (tmp >= timer_usage.max_timer)
#endif
   )
   {
#if TIMER_DEBUG
      TIMER_PRINTF("add %s is error, unknow the para!\r\n", pName);
#endif
      return -1;
   }

   if (runMode == TIMER_RUN_CTN && runCount == 0)
   {
#if TIMER_DEBUG
      TIMER_PRINTF("add %s is error, run count is zero!\r\n", pName);
#endif
      return -1;
   }

   if (0 != timer_tcb[prio])
   {
#if TIMER_DEBUG
      TIMER_PRINTF("the prio task of %d is exist!\r\n", prio);
#endif
      return -1;
   }
   phead1 = freeTimer->next;
   freeTimer->next = headTimer;
   if ((Timer_TypeDef *)0 != headTimer)
      headTimer->prev = freeTimer;

   headTimer = freeTimer;
   freeTimer->prev = (Timer_TypeDef *)0;
   freeTimer = phead1;

   headTimer->name = pName;
   headTimer->p_arg = (void *)p_arg;

   if (runMode == TIMER_RUN_CTN)
      headTimer->remain_run = runCount;

   headTimer->permi_run = runMode;
   headTimer->pFunc = pFunc;
   headTimer->offest_delay = offest_delay;
   headTimer->b_fristRun = 1;
   //prio定义
   headTimer->prio = prio;
   headTimer->x = prio >> 3;
   headTimer->y = prio % 8;

   timer_init_set(headTimer, tDelay);
   timer_tcb[prio] = (Timer_TypeDef *)headTimer;

#if TIMER_RUN_COUNT_STAT
   //资源分配统计
   timer_usage.use_timer++;
   timer_usage.usage_timer = (timer_usage.use_timer * 100.0 / timer_usage.max_timer);
#endif
   return 0;
}

/***************************************************************************************************************************
*@funcName: int8_t timer_taskUnInstall(callBack_Type func_name)
*@describe: 卸载一个任务
*@input:
@func_name: task的任务地址(函数名)
*@return:
@retV: -1 -非法参数  -2 -找不到函数(任务)   0 -删除成功
****************************************************************************************************************************/
int8_t timer_taskUnInstall(callBack_Type func_name)
{
   uint8_t b_flag;
   Timer_TypeDef *phead_copy = headTimer, *phead;

   if ((Timer_TypeDef *)0 == phead_copy || ((callBack_Type)0 == func_name)
#if TIMER_RUN_COUNT_STAT
       || (timer_usage.use_timer <= 0)
#endif
   )
   {
#if TIMER_DEBUG
      TIMER_PRINTF("remove function is error, unknow the para!\r\n");
#endif
      return -1; //unknow task
   }
   b_flag = 0;
   do
   {
      if (phead_copy->pFunc != (callBack_Type)func_name)
      {
         phead_copy = phead_copy->next;
      }
      else
      {
         b_flag = 1; //has found
         break;
      }
   } while ((Timer_TypeDef *)0 != phead_copy);

   if (b_flag)
   {
      if (phead_copy == headTimer)
      {
         phead_copy = headTimer->next;
         headTimer->next = freeTimer;
         freeTimer = headTimer;
         headTimer = phead_copy;
         if (headTimer != (Timer_TypeDef *)0)
         {
            headTimer->prev = (Timer_TypeDef *)0;
         }
      }
      else
      {
         phead = phead_copy->next;
         phead_copy->next = freeTimer;
         freeTimer = phead_copy;
         phead_copy->prev->next = phead;
         if (phead != (Timer_TypeDef *)0)
         {
            phead->prev = phead_copy->prev;
         }
      }
      freeTimer->permi_run = TIMER_STOP_ALL;
      freeTimer->remain_run = 0;
      freeTimer->name = (void *)"???";
      freeTimer->pFunc = (callBack_Type)0;
      timer_tcb[freeTimer->prio] = 0;
      timer_clearReadyTabl(freeTimer->prio);
      freeTimer->prio = 0XFF;

#if TIMER_RUN_COUNT_STAT
      //资源分配统计
      timer_usage.use_timer--;
      timer_usage.usage_timer = (timer_usage.use_timer * 100.0 / timer_usage.max_timer);
#endif
      return 0;
   }
   else
   {
#if TIMER_DEBUG
      TIMER_PRINTF("remove function is error, can't found the function!\r\n");
#endif
      return -2; //don't found
   }
}

/***************************************************************************************************************************
*@funcName: uint8_t timer_getUsage(void)
*@describe: 获取定时器资源使用率
*@input:    None
*@return:   使用率(0-100%)
****************************************************************************************************************************/
#if TIMER_RUN_COUNT_STAT
uint8_t timer_getUsage(void)
{
   return timer_usage.usage_timer;
}
#endif

/***************************************************************************************************************************
*@funcName: void timer_debug_showAllUseTimer(void)
*@describe: 打印定时器任务链信息(包括链路结构信息)
*@input:    None
*@return:   None
****************************************************************************************************************************/
#if TIMER_DEBUG
void timer_debug_showAllUseTimer(void)
{
   TIMER_PRINTF("*********************************Info of Timer***************************************\r\n");
#if TIMER_RUN_COUNT_STAT
   TIMER_PRINTF("Version: V" TIMER_VERSION "\t\t\tTimer_Uage:%d%%\r\n", timer_getUsage());
#else
   TIMER_PRINTF("Version: V" TIMER_VERSION "\r\n");
#endif
   if ((Timer_TypeDef *)0 == headTimer)
   {
      TIMER_PRINTF("no timer is run\r\n");
      return;
   }
   Timer_TypeDef *phead_copy = headTimer, *phead1;

   TIMER_PRINTF("\r\n");
   do
   {
      TIMER_PRINTF("name:\t%s", phead_copy->name);
#if TIMER_RUN_COUNT_STAT
      TIMER_PRINTF("\t\trun_count:\t%d", phead_copy->run_count);
#endif
      TIMER_PRINTF("\t\tdelay:\t%d\r\n", phead_copy->tdelay);
      phead_copy = phead_copy->next;
   } while ((Timer_TypeDef *)0 != phead_copy);
   TIMER_PRINTF("\r\n***********************************Next*******************************************\r\n");
   phead_copy = headTimer;
   do
   {
      TIMER_PRINTF("%s -> ", phead_copy->name);
      phead1 = phead_copy;
      phead_copy = phead_copy->next;
   } while ((Timer_TypeDef *)0 != phead_copy);
   TIMER_PRINTF("%d", (uint32_t)phead_copy->next);
   TIMER_PRINTF("\r\n\r\n***********************************Prev*******************************************\r\n");
   do
   {
      TIMER_PRINTF("%s -> ", phead1->name);
      phead1 = phead1->prev;
   } while ((Timer_TypeDef *)0 != phead1);
   TIMER_PRINTF("%d", (uint32_t)phead1->prev);
   TIMER_PRINTF("\r\n\r\n***********************************FreeList*******************************************\r\n");
   phead1 = freeTimer;
   while ((Timer_TypeDef *)0 != phead1)
   {
      TIMER_PRINTF("%s -> ", phead1->name);
      phead1 = phead1->next;
   }
   TIMER_PRINTF("%d", (uint32_t)phead1->next);
   TIMER_PRINTF("\r\n\r\n\r\n\r\n");
}
#endif

/***************************************************************************************************************************
*@funcName: void timer_schedRunning(void)
*@describe: 任务调度 (时间到达 任务分配cpu执行)
*@input:   None
*@return:  None
****************************************************************************************************************************/
void timer_schedRunning(void)
{
   Timer_TypeDef *phead_copy = headTimer;

   if ((Timer_TypeDef *)0 == phead_copy)
   {
      return; //none task
   }
   do
   {
      if (is_timer_ready(phead_copy) && phead_copy->pFunc != (callBack_Type)0)
      {
         timer_setReadyTabl(phead_copy->prio);
         if (phead_copy->permi_run == TIMER_RUN_CTN)
         {
            if (!(--phead_copy->remain_run))
            {
               timer_taskUnInstall(phead_copy->pFunc);
            }
         }
      }
      phead_copy = phead_copy->next;
   } while ((Timer_TypeDef *)0 != phead_copy);
   //task execute
   timer_taskExecute();
}

/***************************************************************************************************************************
*@funcName: void timer_initAll(void)
*@describe: 初始化定时器
*@input:    None
*@return:   None
****************************************************************************************************************************/
void timer_initAll(void)
{
   uint8_t count;

   sys_timer[0].next = (Timer_TypeDef *)0;
   for (count = 0; count < TIMER_COUNT - 1; count++)
   {
      sys_timer[count + 1].next = &sys_timer[count];

      sys_timer[count].name = (void *)"???";
      sys_timer[count].permi_run = TIMER_STOP_ALL;
      sys_timer[count].pFunc = (void *)0;
      timer_init_set(&sys_timer[count], 0);
      sys_timer[count].prio = 0xff;
#if TIMER_RUN_COUNT_STAT
      sys_timer[count].run_count = 0;
#endif
   }
   freeTimer = &sys_timer[count];
   headTimer = (Timer_TypeDef *)0;

   sys_timer[count].name = (void *)"???";
   sys_timer[count].permi_run = TIMER_STOP_ALL;
   sys_timer[count].pFunc = (void *)0;
   sys_timer[count].prio = 0xff;
   timer_init_set(&sys_timer[count], 0);

#if TIMER_RUN_COUNT_STAT
   sys_timer[count].run_count = 0;
   timer_usage.max_timer = TIMER_COUNT;
   timer_usage.use_timer = 0;
   timer_usage.usage_timer = 0;
#endif
   readyGrp = 0x00;
   memset(&timer_tcb[0], 0, sizeof(timer_tcb));
   memset(readyTabl, 0, sizeof(readyTabl));
}

//end of file
