
/**********************************************************************************************************
 *@fileName: threadM_mem.c
 *@platform: stm32l432kbxx + mdk5.2.26.0
 *@version:  v1.0.0
 *@describe: Implement the encapsulation of dynamic application memory by thread manager
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190729         EtcFly          Create  file
**********************************************************************************************************/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "thread_manager.h"




extern threadManager_Type s_threadM;


threadUnit_Type * threadFound(rt_list_t *l, rt_thread_t pthread)
{
    rt_list_t *p;
    threadUnit_Type *unit;
    
    RT_ASSERT(RT_NULL != l);

    if (rt_list_isempty(l))
    {
        return RT_NULL;
    }

    p = l->next;

    rt_enter_critical();

    while (l != p)
    {
        unit = rt_list_entry(p, threadUnit_Type, list);
        if (unit->thread == pthread) {
			rt_exit_critical();
            return unit;
        }
        p = p->next;
    }

    rt_exit_critical();

    return RT_NULL;
}


static threadMem_Type  *memFound(rt_list_t *l, void *mem)
{
    if (rt_list_isempty(l))
    {
        return RT_NULL;
    }

    rt_list_t *p;

    p = l->next;
    while (l != p)
    {
        threadMem_Type *memUnit;

        memUnit = rt_list_entry(p, threadMem_Type, list);
        if (memUnit->mem == mem)
        {
            return memUnit;
        }

        p = p->next;
    }

    return RT_NULL;
}

void *threadM_malloc(rt_size_t nbyte)
{
    rt_thread_t thread;
    threadUnit_Type *unit;
    void *mem;

    thread = rt_thread_self();
    RT_ASSERT(RT_NULL != thread);

    mem = rt_malloc(nbyte);
    if (RT_NULL == mem)
    {
        return  RT_NULL;
    }

    unit = threadFound(&s_threadM.norThreadList, thread);

    if (RT_NULL == unit)
    {
        return mem;
    }
	
	threadMem_Type *memUnit;
	
	memUnit = (threadMem_Type *)(void *)rt_malloc(sizeof(threadMem_Type));
	
	RT_ASSERT(RT_NULL != memUnit);

	memUnit->mem = mem;
	rt_list_insert_after(&unit->memList, &memUnit->list);

    return mem;
}

void *threadM_calloc(rt_size_t count, rt_size_t size)
{
    void *mem;

    mem = threadM_malloc(count*size);

    if (RT_NULL != mem) 
    {
        rt_memset(mem, 0, count*size);
    }

    return mem;
}

void *threadM_relloc(void *rmem, rt_size_t newsize)
{
    void *mem;
    rt_thread_t thread;
    threadUnit_Type *unit;
    threadMem_Type  *memUnit;

    thread = rt_thread_self();

    RT_ASSERT(RT_NULL != rmem);  
    RT_ASSERT(RT_NULL != thread);  

    unit = threadFound(&s_threadM.norThreadList, thread);
    
    if (RT_NULL == unit) {
        mem = rt_realloc(rmem, newsize);
        return mem;
    }
    
    memUnit = memFound(&unit->memList, rmem);
    mem = rt_realloc(rmem, newsize);

    if (RT_NULL != memUnit)
    {
        memUnit->mem = (void *)mem;
        return mem;
    }

    threadMem_Type *pmem;
    pmem = rt_malloc(sizeof(threadMem_Type));
    if (RT_NULL == pmem)
    {
        return RT_NULL;
    }

    rt_base_t level;
    level = rt_hw_interrupt_disable();
    rt_list_insert_after(&unit->memList, &pmem->list);  
    rt_hw_interrupt_enable(level);

    return pmem;
}


void threadM_free(void *rmem)
{
    rt_list_t *l;
    threadUnit_Type *unit;
    threadMem_Type  *memUnit = RT_NULL;

    RT_ASSERT(RT_NULL != rmem);

    rt_enter_critical();

    l = &s_threadM.norThreadList;
    if (!rt_list_isempty(l)) {
        l = l->next;
        while (&s_threadM.norThreadList != l)
        {
            unit = rt_list_entry(l, threadUnit_Type, list);
            memUnit = memFound(&unit->memList, rmem);

            if (RT_NULL != memUnit)
            {
                break;
            }
            l = l->next;
        }
    } 

    if (RT_NULL == memUnit)
    {
        l = &s_threadM.errThreadList;
        if (!rt_list_isempty(l))  {
            l = l->next;
            while (&s_threadM.errThreadList != l)
            {
                unit = rt_list_entry(l, threadUnit_Type, list);
                memUnit = memFound(&unit->memList, rmem);

                if (RT_NULL != memUnit)
                {
                    break;
                }
                l = l->next;
            }
        }
    }


    rt_exit_critical();

    if (RT_NULL != memUnit && memUnit->mem == rmem)
    {
        memUnit->mem = RT_NULL;

        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_list_remove(&memUnit->list);
        rt_hw_interrupt_enable(level);
        
        rt_free(memUnit);
    }

    rt_free(rmem);
}


















