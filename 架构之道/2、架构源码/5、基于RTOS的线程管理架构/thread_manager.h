
/**********************************************************************************************************
 *@fileName: thread_manager.h
 *@platform: stm32l432kbxx + mdk5.2.26.0
 *@version:  v1.0.0
 *@describe: Implementation of thread monitoring and management operations
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190725         EtcFly          Create  file
**********************************************************************************************************/
#ifndef _THREAD_MANAGER_H
#define _THREAD_MANAGER_H




#include "threadM_mem.h"
#include "threadM_ipc.h"


#define  THREAD_MANAGER_PRIO                     PRIORITY_THREADM_TASK       
#define  THREAD_MANAGER_TICK                     TCIK_THREAD_TASK


typedef struct 
{
    void      *mem;
    rt_list_t list;
}threadMem_Type;

typedef struct 
{
    void      *ipc;   
    rt_list_t list;
}threadIpc_Type;


typedef struct 
{
    rt_list_t            list;    //linking thread unit
    rt_list_t            memList; //linking threadMem_Type
    rt_list_t            ipcList; //linking threadIpc_Type
    rt_thread_t          thread;

    uint32_t             init_timeout;
    uint32_t             timeout;
    void (*callback)(void *arg);
    void *p_arg;
}threadUnit_Type;

typedef struct 
{
    char      name[RT_NAME_MAX];  //name of error thread  
    uint8_t   prio;     //priority of error thread
    uint8_t   restartCount; //record count of this thread
    rt_err_t  errCode;  //the reason of last thread restart
    rt_list_t list;
}threadRecord_Type;

typedef struct 
{
    rt_list_t            norThreadList;  //linking normal threadUnit_Type
    rt_list_t            errThreadList;  //linking error threadUnit_Type
    rt_list_t            recordList;     //linking threadRecord_Type
    uint8_t              count; //thread count in thread manager
}threadManager_Type;


#ifdef _cpluscplus
extern "C" {
#endif



threadUnit_Type * threadM_register(uint32_t timeout, void (*callback)(void *arg), void *arg);
void threadM_update(threadUnit_Type *unit);



#ifdef _cpluscplus
}
#endif  //_cpluscplus

#endif //_THREAD_MANAGER_H 



