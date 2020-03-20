
/**********************************************************************************************************
 *@fileName: thread_manager.c
 *@platform: stm32l432kbxx + mdk5.2.26.0
 *@version:  v1.0.0
 *@describe: Implementation of thread monitoring and management operations
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190725         EtcFly          Create  file
**********************************************************************************************************/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "thread_manager.h"
#include "app_config.h"



typedef struct 
{
    char *name;
    void (*entry)(void *parameter);
    void *parameter; 
    rt_uint32_t stack_size;
    rt_uint8_t  priority;
    rt_uint32_t tick;
}threadInfo_Type;


threadManager_Type s_threadM;



int threadManager_init(void)
{
    rt_memset((void *)&s_threadM, 0, sizeof(s_threadM));

    rt_list_init(&s_threadM.errThreadList);
    rt_list_init(&s_threadM.norThreadList);   
    rt_list_init(&s_threadM.recordList);
	return 0;
}
INIT_ENV_EXPORT(threadManager_init);


static rt_err_t threadM_threadUnit_init(threadUnit_Type *unit,  rt_thread_t thread,
                                    void (*callback)(void *arg), void *arg,
                                    uint32_t timeout)
{
    rt_memset((void *)unit, 0, sizeof(threadUnit_Type));   

    unit->callback = callback;
    unit->p_arg = arg;
    unit->timeout = timeout;
    unit->init_timeout = timeout;  
    unit->thread = thread;

    rt_list_init(&unit->list); 
    rt_list_init(&unit->ipcList);
    rt_list_init(&unit->memList);

    return RT_EOK;
}


void threadM_update(threadUnit_Type *unit)
{
    unit->timeout = unit->init_timeout;   
}


threadUnit_Type * threadM_register(uint32_t timeout, void (*callback)(void *arg), void *arg)
{
    rt_base_t   tmp;
    rt_thread_t pthread;

    RT_ASSERT(RT_NULL != callback);
    RT_ASSERT(timeout > 0);

    threadUnit_Type *pUnit = (threadUnit_Type *)rt_malloc(sizeof(threadUnit_Type));

    if (RT_NULL == pUnit) {
        return RT_NULL;
    }

    pthread = rt_thread_self();
    RT_ASSERT(RT_NULL != pthread);

    if (threadM_threadUnit_init(pUnit, pthread, callback, arg, timeout) != RT_EOK)
    {
        rt_free(pUnit);
        return RT_NULL;
    }
    
    tmp = rt_hw_interrupt_disable();

    rt_list_insert_after(&s_threadM.norThreadList, &pUnit->list);
    s_threadM.count ++;

    rt_hw_interrupt_enable(tmp);

	return pUnit;
}


static rt_err_t threadM_unregister(threadUnit_Type *unit)
{
    unit->thread = RT_NULL;
    unit->callback = RT_NULL;
	
	rt_list_init(&unit->ipcList);
	rt_list_init(&unit->memList);
	rt_list_init(&unit->list);

    rt_free(unit);

    return RT_EOK;
}


static threadRecord_Type * threadM_listRecordFound(rt_list_t *l,  char *name)
{
    threadRecord_Type *pRecord;

    if (rt_list_isempty(l))
    {
        return RT_NULL;
    }

    l = l->next;
    while (&s_threadM.recordList != l)
    {
        pRecord = rt_list_entry(l, threadRecord_Type, list);
        if (rt_strncmp(pRecord->name, name, RT_NAME_MAX) == 0)
        {
            return pRecord;
        }
        l = l->next;
    }

    return RT_NULL;
}


static rt_err_t threadM_addNewRecord(threadUnit_Type *unit)
{
    rt_base_t tmp;

    RT_ASSERT(RT_NULL != unit);

    threadRecord_Type *pRecord = (threadRecord_Type *)rt_malloc(sizeof(threadRecord_Type));
    if (RT_NULL == pRecord) {
        rt_kprintf("malloc a new Record memory space fail!\r\n");
        return RT_ERROR;
    }
	
	rt_kprintf("add a record, size:%d\r\n", sizeof(threadRecord_Type));
    tmp = rt_hw_interrupt_disable();
    rt_list_insert_after(&s_threadM.recordList, &pRecord->list);
    rt_hw_interrupt_enable(tmp);

    rt_memcpy(pRecord->name, unit->thread->name, RT_NAME_MAX);
    pRecord->prio = unit->thread->current_priority;
    pRecord->errCode = unit->thread->error;
    pRecord->restartCount = 1;

    return RT_EOK;
}


static void threadM_reloadThreadInfo(rt_thread_t thread, threadInfo_Type *info)
{
    info->entry = (void (*)(void *))(thread->entry);
    info->name = thread->name;
    info->parameter = thread->parameter;
    info->stack_size = thread->stack_size;
    info->priority = thread->init_priority;
    info->tick = thread->init_tick;
}


static void threadM_memFree(rt_list_t *plist)
{
    RT_ASSERT(RT_NULL != plist);

    plist = plist->next;
    while (!rt_list_isempty(plist))
    {
        threadMem_Type *memUnit = rt_list_entry(plist, threadMem_Type, list);

        plist = plist->next;
        
        rt_base_t tmp;
        tmp = rt_hw_interrupt_disable();
        rt_list_remove(&memUnit->list);
        rt_hw_interrupt_enable(tmp);

		if (RT_NULL != memUnit->mem)
		{
			rt_free(memUnit->mem);
		}
        rt_free(memUnit);
    }

}


static void threadM_ipcFree(rt_list_t *plist)
{
    RT_ASSERT(RT_NULL != plist);

    plist = plist->next;
    while (!rt_list_isempty(plist))
    {
        threadIpc_Type *ipcUnit = rt_list_entry(plist, threadIpc_Type, list);

        plist = plist->next;

        rt_base_t tmp;
        tmp = rt_hw_interrupt_disable();
        rt_list_remove(&ipcUnit->list);
        rt_hw_interrupt_enable(tmp);

        rt_uint8_t type = ((rt_object_t)ipcUnit->ipc)->type;
        switch (type)
        {
            case RT_Object_Class_Thread:
                rt_thread_delete((rt_thread_t)ipcUnit->ipc);
				rt_kprintf("delete thread...............\r\n");
                break;

            case RT_Object_Class_Semaphore:
                rt_sem_delete((rt_sem_t)ipcUnit->ipc);
				rt_kprintf("delete sem...............\r\n");
                break;

            case RT_Object_Class_Mutex:
                rt_mutex_delete((rt_mutex_t)ipcUnit->ipc);
				rt_kprintf("delete mutex...............\r\n");
                break;

            case RT_Object_Class_Event:
                rt_event_delete((rt_event_t)ipcUnit->ipc);
				rt_kprintf("delete event...............\r\n");
                break;

            case RT_Object_Class_MailBox:
                rt_mb_delete((rt_mailbox_t)ipcUnit->ipc);
				rt_kprintf("delete mailbox...............\r\n");
                break;

            case RT_Object_Class_MessageQueue:
                rt_mq_delete((rt_mq_t)ipcUnit->ipc);
				rt_kprintf("delete message queue...............\r\n");
                break;

            case RT_Object_Class_Timer:
                rt_timer_delete((rt_timer_t)ipcUnit->ipc);
				rt_kprintf("delete timer...............\r\n");
                break;
                
            default: 
				rt_kprintf("invalid ipc link to be delete ..............\r\n");
				break;

        }
		rt_free(ipcUnit);
    }
}


static rt_err_t threadM_recoverThread(threadUnit_Type *unit, threadInfo_Type *info)
{
    rt_thread_t pthread;
    rt_err_t    errCode;

	errCode = RT_ERROR;
    pthread = rt_thread_create(info->name, info->entry, info->parameter, info->stack_size, info->priority, info->tick);
    if (RT_NULL != pthread) {
        if (rt_thread_startup(pthread) == RT_EOK) {
            errCode = RT_EOK;
        }
    }
	
	//old thread resource release 
	if (!rt_list_isempty(&unit->memList)) {
		threadM_memFree(&unit->memList);
	}

	if (!rt_list_isempty(&unit->ipcList)) {
		threadM_ipcFree(&unit->ipcList);
	}

	threadM_unregister(unit);
    return errCode;
}

static rt_err_t threadM_restartThread(threadUnit_Type *unit)
{
    if (RT_NULL != unit->callback) {
        unit->callback(unit->p_arg);
    }

    threadInfo_Type info;
    threadM_reloadThreadInfo(unit->thread, &info);
    if (rt_thread_delete(unit->thread) != RT_EOK) {
        rt_kprintf("the error thread %s delete fail !\r\n", unit->thread->name);
        
        // rt_base_t tmp;
        // tmp = rt_hw_interrupt_disable();
        // rt_list_remove(&unit->list);
        // rt_hw_interrupt_enable(tmp);
    } else {
        rt_kprintf("the error thread %s delete ok !\r\n", unit->thread->name);  
        
        rt_base_t tmp;
        tmp = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        s_threadM.count --;

        rt_hw_interrupt_enable(tmp); 

        if (threadM_recoverThread(unit, &info) != RT_EOK) {
            rt_kprintf("the error thread %s receover fail!\r\n", info.name);
        } else rt_kprintf("the error thread %s receover ok!\r\n", info.name);
    }

	return RT_EOK;
}



//update the status of normal thread
static void threadM_updateStatus(void)
{
    rt_list_t *list_index;
    threadUnit_Type *pUint;
    
    list_index = &s_threadM.norThreadList;
    if (!rt_list_isempty(list_index))
    {
        list_index = list_index->next;
        while (&s_threadM.norThreadList != list_index)
        {
            pUint = rt_list_entry(list_index, threadUnit_Type, list);

            if (pUint->timeout > 0) {
                pUint->timeout --;
            }

            if (pUint->timeout == 0)
            {
                rt_base_t tmp;
				
				list_index = list_index->next;
				
                tmp = rt_hw_interrupt_disable();

                rt_list_remove(&pUint->list);
                rt_list_insert_after(&s_threadM.errThreadList, &pUint->list);

                rt_hw_interrupt_enable(tmp);
            }
			else  list_index = list_index->next;
        }
    }
}


static void threadManager_thread(void *p_arg)
{
	
    while (1)
    {
        //thread status update
        threadM_updateStatus();

        //error thread solve
        while (!rt_list_isempty(&s_threadM.errThreadList))
        {
            threadUnit_Type *thread;
            threadRecord_Type *pThread;
            
            thread = rt_list_first_entry(&s_threadM.errThreadList, threadUnit_Type, list);
            pThread = threadM_listRecordFound(&s_threadM.recordList, thread->thread->name);

            rt_kprintf("thread manager found address:0X%08X\r\n", pThread);
            //record update
            if (RT_NULL == pThread) {
                //add a new record 
                threadM_addNewRecord(thread);
            } else {
                pThread->restartCount++;
                pThread->errCode = thread->thread->error;
            }

            //error thread recover
            threadM_restartThread(thread);
        }
		
        rt_thread_mdelay(500);
    }
}


static int threadMange_init(void)
{
    rt_err_t err;
    rt_thread_t thread;

    thread = rt_thread_create("threadM", threadManager_thread, (void *)0, 1024, THREAD_MANAGER_PRIO, THREAD_MANAGER_TICK);
    
    if (RT_NULL != thread) {
        rt_kprintf("thread manager create \r\t\t\t\t......................... [ok]\r\n");
    } else {
        rt_kprintf("thread manager create \r\t\t\t\t......................... [fail]\r\n");
    }

    err = rt_thread_startup(thread);
    if (RT_EOK != err) {
        rt_kprintf("thread manager start \r\t\t\t\t......................... [fail]\r\n");
    }  else  rt_kprintf("thread manager start \r\t\t\t\t......................... [ok]\r\n");
	
	return 0;
}
INIT_ENV_EXPORT(threadMange_init);


//return number of linked list items
static rt_base_t thread_listCount(rt_list_t *l)
{
    rt_base_t count = 0;
    if (rt_list_isempty(l)) {
        return count;
    }

    rt_list_t *m;
    
    m = l;
    l = l->next;

    while (m != l)
    {
        l = l->next;
        count ++;
    }
    
    return count;
}

/**
 * @describe: show thread manager status information and print it 
 * 
 * @para: list the items which need to print 
 *        type the type of list
 * 
 * @return: return the result Whether the list is empty, 1 is empty otherwise return 0
 */
static uint8_t threadM_showStatus(rt_list_t *list, const char *type)
{
    uint8_t b_empty = 0;

    if (rt_list_isempty(list))
    {
        return b_empty;
    }

    RT_ASSERT(RT_NULL != type);

    rt_list_t *l;

    b_empty = 1;
    l = list->next;

    while (list != l)
    {
        threadUnit_Type *pUnit = rt_list_entry(l, threadUnit_Type, list);

        rt_kprintf("%-*.*s   %3d      %3d       %3d      %3d       %.*s\t\n", 
								RT_NAME_MAX, RT_NAME_MAX, pUnit->thread->name, 
                                pUnit->thread->current_priority, pUnit->timeout, thread_listCount(&pUnit->ipcList), 
								thread_listCount(&pUnit->memList), rt_strlen(type), type);
        l = l->next;
    }

    return b_empty;
}


#ifdef RT_USING_FINSH

int threadMange_showInfo(void)
{
    rt_kprintf("\r+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
    rt_kprintf("\r  name          prio           reCount            errCode\r\n");
    rt_kprintf("\r--------       ------          -------            -------\r\n");
    rt_list_t *list = &s_threadM.recordList;

    if (!rt_list_isempty(list))
    {
        list = list->next;
        while (&s_threadM.recordList != list)
        {
            threadRecord_Type *recordUnit = rt_list_entry(list, threadRecord_Type, list);

            list = list->next;

            rt_kprintf("%-*.*s \t%3d     \t%3d \t\t0X%08X\t\n", RT_NAME_MAX, RT_NAME_MAX, recordUnit->name, 
			                      recordUnit->prio, recordUnit->restartCount, recordUnit->errCode);
        }
    } else {
		rt_kprintf("\r\n");
	}             
    rt_kprintf("\r+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");

	return 0;
}
MSH_CMD_EXPORT_ALIAS(threadMange_showInfo, threadM_showInfo, printf thread information);


int thread_showLinkStatus(void)
{
    rt_kprintf("\r+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
    rt_kprintf("\r thread    prio    timeout  ipcCount  memCount   type\r\n");
    rt_kprintf("\r--------  ------   -------  -------   --------   ----\r\n");


    uint8_t b_linkEmpty;

    b_linkEmpty = 0;

    b_linkEmpty = threadM_showStatus(&s_threadM.norThreadList, "Norm");
    b_linkEmpty |= threadM_showStatus(&s_threadM.errThreadList, "Erro");

    if (b_linkEmpty) {
        rt_kprintf("\r\n");
    }
    rt_kprintf("\r+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n"); 
	
	return 0;
}
MSH_CMD_EXPORT_ALIAS(thread_showLinkStatus, threadM_showStatus, printf thread link status);

#endif














