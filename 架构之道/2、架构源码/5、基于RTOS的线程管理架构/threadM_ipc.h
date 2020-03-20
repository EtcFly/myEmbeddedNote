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
#ifndef _THREADM_IPC_H
#define _THREADM_IPC_H








#ifdef _cplusplus
extern "C" {
#endif


rt_thread_t threadM_thread_create(const char *name,
                                  void (*entry)(void *parameter),
                                  void       *parameter,
                                  rt_uint32_t stack_size,
                                  rt_uint8_t  priority,
                                  rt_uint32_t tick);
rt_err_t threadM_thread_delete(rt_thread_t thread);
rt_sem_t threadM_sem_create(const char *name, rt_uint32_t value, rt_uint8_t flag);
rt_err_t threadM_sem_delete(rt_sem_t sem);
rt_mutex_t threadM_mutex_create(const char *name, rt_uint8_t flag);
rt_err_t threadM_mutex_delete(rt_mutex_t mutex);
rt_event_t threadM_event_create(const char *name, rt_uint8_t flag);
rt_err_t threadM_event_delete(rt_event_t event);
rt_mailbox_t threadM_mb_create(const char *name, rt_size_t size, rt_uint8_t flag);
rt_err_t threadM_mb_delete(rt_mailbox_t mb);
rt_mq_t threadM_mq_create(const char *name,
                          rt_size_t   msg_size,
                          rt_size_t   max_msgs,
                          rt_uint8_t  flag);
rt_err_t threadM_mq_delete(rt_mq_t mq);
rt_timer_t threadM_timer_create(const char *name,
                                void (*timeout)(void *parameter),
                                void       *parameter,
                                rt_tick_t   time,
                                rt_uint8_t  flag);
rt_err_t threadM_timer_delete(rt_timer_t timer);






#ifdef _cplusplus
}
#endif //_cplusplus
#endif //_THREADM_IPC_H


