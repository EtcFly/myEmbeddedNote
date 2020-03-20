/**********************************************************************************************************
 *@fileName: threadM_ipc.c
 *@platform: stm32l432kbxx + mdk5.2.26.0
 *@version:  v1.0.0
 *@describe: The encapsulation of dynamic resources is realized through thread manager(Sem, Mutex, Mailbox,
              message queue and so on)
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190731         EtcFly          Create  file
**********************************************************************************************************/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "thread_manager.h"





extern threadUnit_Type * threadFound(rt_list_t *l, rt_thread_t pthread);

extern threadManager_Type s_threadM;


static threadIpc_Type *threadIpcFound(rt_list_t *l, void *ipc)
{
    if (rt_list_isempty(l))
    {
        return RT_NULL;
    }

    rt_list_t *p;
    threadIpc_Type *unitIpc;

    p = l->next;
    while (l != p)
    {
        unitIpc = rt_list_entry(p, threadIpc_Type, list);
        if (unitIpc->ipc == ipc) {
            return unitIpc;
        }
        p = p->next;
    }

    return RT_NULL;
}

/**
 * @describe: found ipc in thread manager
 * 
 * @para: ipc the targe need to found
 * 
 * @return: the targe threadIpc_Type which hold ipc 
 */

static threadIpc_Type * threadLink_found(void *ipc)
{
    threadUnit_Type *unit;
    threadIpc_Type *ipcUnit = RT_NULL;

    rt_enter_critical();

    if (!rt_list_isempty(&s_threadM.norThreadList))
    {
        rt_list_t *l;

        l = s_threadM.norThreadList .next;
        while (&s_threadM.norThreadList != l)
        {
            unit = rt_list_entry(l, threadUnit_Type, list);
            ipcUnit = threadIpcFound(&unit->ipcList, ipc);
            if (RT_NULL != ipcUnit) {
                break;
            }   
            l = l->next;
        }
    }

    if (RT_NULL == ipcUnit)
    {
        rt_list_t *l;

        l = s_threadM.errThreadList .next;
        while (&s_threadM.errThreadList != l)
        {
            unit = rt_list_entry(l, threadUnit_Type, list);
            ipcUnit = threadIpcFound(&unit->ipcList, ipc);
            if (RT_NULL != ipcUnit) {
                break;
            }   
            l = l->next;
        }    
    }

    rt_exit_critical();
    
    return ipcUnit;
}


/*************** thread ********************/
rt_thread_t threadM_thread_create(const char *name,
                                  void (*entry)(void *parameter),
                                  void       *parameter,
                                  rt_uint32_t stack_size,
                                  rt_uint8_t  priority,
                                  rt_uint32_t tick)
{
    rt_thread_t thread;
    rt_thread_t pthread;
	threadUnit_Type *unit;
	
    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != thread);

    pthread = rt_thread_create(name, entry, parameter, stack_size, priority, tick);

    if (RT_NULL == pthread)
    {
        return pthread;
    }

	unit = threadFound(&s_threadM.norThreadList, thread);
	
	if (RT_NULL == unit)
	{
		return pthread;
	}
	
    threadIpc_Type *ipcUnit;

    ipcUnit = (threadIpc_Type *)rt_malloc(sizeof(threadIpc_Type));
    if (RT_NULL == ipcUnit)
    {
        rt_thread_delete(pthread);
        return RT_NULL;
    }

    ipcUnit->ipc = pthread;
    
    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->ipcList, &ipcUnit->list);
    rt_hw_interrupt_enable(level);

    return pthread;
}

rt_err_t threadM_thread_delete(rt_thread_t thread)
{
    rt_err_t err;
    threadIpc_Type *unit;


    unit = threadLink_found(thread);
    
    if (RT_NULL != unit)
    {
        unit->ipc = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        rt_hw_interrupt_enable(level);

        rt_free(unit);
    }

    err =  rt_thread_delete(thread);   

    return err;
}

/************** Sem ************************/
rt_sem_t threadM_sem_create(const char *name, rt_uint32_t value, rt_uint8_t flag)
{
    rt_thread_t thread;
    rt_sem_t sem;

    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != thread);

    sem = rt_sem_create(name, value, flag);
    if (RT_NULL == sem)
    {
        return RT_NULL;
    }

    threadUnit_Type *unit;

    unit = threadFound(&s_threadM.norThreadList, thread);	
	if (RT_NULL == unit)
	{
		return sem;
	}

    threadIpc_Type *ipc;

    ipc = (threadIpc_Type *)rt_malloc(sizeof(threadIpc_Type));
    if (RT_NULL == ipc)
    {
        rt_sem_delete(sem);
        return RT_NULL;
    }

    ipc->ipc = sem;

    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->ipcList, &ipc->list);
    rt_hw_interrupt_enable(level);

    return sem;
}

rt_err_t threadM_sem_delete(rt_sem_t sem)
{
    rt_err_t err;
    threadIpc_Type *unit;


    unit = threadLink_found(sem);
    
    if (RT_NULL != unit)
    {
        unit->ipc = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        rt_hw_interrupt_enable(level);

        rt_free(unit);
    }

    err =  rt_sem_delete(sem);   

    return err;
}

/************** Mutex ************************/
rt_mutex_t threadM_mutex_create(const char *name, rt_uint8_t flag)
{
    rt_thread_t thread;
    rt_mutex_t mutex;

    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != thread);

    mutex = rt_mutex_create(name, flag);
    if (RT_NULL == mutex)
    {
        return RT_NULL;
    }

    threadUnit_Type *unit;

    unit = threadFound(&s_threadM.norThreadList, thread);	
	if (RT_NULL == unit)
	{
		return mutex;
	}

    threadIpc_Type *ipc;

    ipc = (threadIpc_Type *)rt_malloc(sizeof(threadIpc_Type));
    if (RT_NULL == ipc)
    {
        rt_mutex_delete(mutex);
        return RT_NULL;
    }

    ipc->ipc = mutex;

    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->ipcList, &ipc->list);
    rt_hw_interrupt_enable(level);

    return mutex;
}

rt_err_t threadM_mutex_delete(rt_mutex_t mutex)
{
    rt_err_t err;
    threadIpc_Type *unit;


    unit = threadLink_found(mutex);
    
    if (RT_NULL != unit)
    {
        unit->ipc = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        rt_hw_interrupt_enable(level);

        rt_free(unit);
    }

    err =  rt_mutex_delete(mutex);   

    return err;
}

/************** Event ************************/
rt_event_t threadM_event_create(const char *name, rt_uint8_t flag)
{
    rt_thread_t thread;
    rt_event_t event;

    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != thread);

    event = rt_event_create(name, flag);
    if (RT_NULL == event)
    {
        return RT_NULL;
    }

    threadUnit_Type *unit;

    unit = threadFound(&s_threadM.norThreadList, thread);	
	if (RT_NULL == unit)
	{
		return event;
	}

    threadIpc_Type *ipc;

    ipc = (threadIpc_Type *)rt_malloc(sizeof(threadIpc_Type));
    if (RT_NULL == ipc)
    {
        rt_event_delete(event);
        return RT_NULL;
    }

    ipc->ipc = event;

    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->ipcList, &ipc->list);
    rt_hw_interrupt_enable(level);

    return event;
}

rt_err_t threadM_event_delete(rt_event_t event)
{
    rt_err_t err;
    threadIpc_Type *unit;


    unit = threadLink_found(event);
    
    if (RT_NULL != unit)
    {
        unit->ipc = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        rt_hw_interrupt_enable(level);

        rt_free(unit);
    }

    err =  rt_event_delete(event);   

    return err;
}

/************** Mailbox ************************/

rt_mailbox_t threadM_mb_create(const char *name, rt_size_t size, rt_uint8_t flag)
{
    rt_thread_t thread;
    rt_mailbox_t mb;

    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != thread);

    mb = rt_mb_create(name, size, flag);
    if (RT_NULL == mb)
    {
        return RT_NULL;
    }

    threadUnit_Type *unit;

    unit = threadFound(&s_threadM.norThreadList, thread);	
	if (RT_NULL == unit)
	{
		return mb;
	}

    threadIpc_Type *ipc;

    ipc = (threadIpc_Type *)rt_malloc(sizeof(threadIpc_Type));
    if (RT_NULL == ipc)
    {
        rt_mb_delete(mb);
        return RT_NULL;
    }

    ipc->ipc = mb;

    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->ipcList, &ipc->list);
    rt_hw_interrupt_enable(level);

    return mb;
}

rt_err_t threadM_mb_delete(rt_mailbox_t mb)
{
   rt_err_t err;
    threadIpc_Type *unit;


    unit = threadLink_found(mb);
    
    if (RT_NULL != unit)
    {
        unit->ipc = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        rt_hw_interrupt_enable(level);

        rt_free(unit);
    }

    err =  rt_mb_delete(mb);   

    return err;
}

/************** Malibox Queue ************************/

rt_mq_t threadM_mq_create(const char *name,
                          rt_size_t   msg_size,
                          rt_size_t   max_msgs,
                          rt_uint8_t  flag)
{
    rt_thread_t thread;
    rt_mq_t mq;

    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != thread);

    mq = rt_mq_create(name, msg_size, max_msgs, flag);
    if (RT_NULL == mq)
    {
        return RT_NULL;
    }

    threadUnit_Type *unit;

    unit = threadFound(&s_threadM.norThreadList, thread);	
	if (RT_NULL == unit)
	{
		return mq;
	}

    threadIpc_Type *ipc;

    ipc = (threadIpc_Type *)rt_malloc(sizeof(threadIpc_Type));
    if (RT_NULL == ipc)
    {
        rt_mq_delete(mq);
        return RT_NULL;
    }

    ipc->ipc = mq;

    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->ipcList, &ipc->list);
    rt_hw_interrupt_enable(level);

    return mq;    
}

rt_err_t threadM_mq_delete(rt_mq_t mq)
{
   rt_err_t err;
    threadIpc_Type *unit;


    unit = threadLink_found(mq);
    
    if (RT_NULL != unit)
    {
        unit->ipc = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        rt_hw_interrupt_enable(level);

        rt_free(unit);
    }

    err =  rt_mq_delete(mq);   

    return err;
}

/************** timer ************************/
rt_timer_t threadM_timer_create(const char *name,
                                void (*timeout)(void *parameter),
                                void       *parameter,
                                rt_tick_t   time,
                                rt_uint8_t  flag)
{
    rt_thread_t thread;
    rt_timer_t  timer;

    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != thread);

    timer = rt_timer_create(name, timeout, parameter, time, flag);
    if (RT_NULL == timer)
    {
        return RT_NULL;
    }

    threadUnit_Type *unit;

    unit = threadFound(&s_threadM.norThreadList, thread);	
	if (RT_NULL == unit)
	{
		return timer;
	}

    threadIpc_Type *ipc;

    ipc = (threadIpc_Type *)rt_malloc(sizeof(threadIpc_Type));
    if (RT_NULL == ipc)
    {
        rt_timer_delete(timer);
        return RT_NULL;
    }

    ipc->ipc = timer;

    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->ipcList, &ipc->list);
    rt_hw_interrupt_enable(level);

    return timer; 
}

rt_err_t threadM_timer_delete(rt_timer_t timer)
{
    rt_err_t err;
    threadIpc_Type *unit;


    unit = threadLink_found(timer);
    
    if (RT_NULL != unit)
    {
        unit->ipc = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&unit->list);
        rt_hw_interrupt_enable(level);

        rt_free(unit);
    }

    err =  rt_timer_delete(timer);   

    return err;
}


