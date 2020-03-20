/**********************************************************************************************************
 *@fileName: threadM_mem.h
 *@platform: stm32l432kbxx + mdk5.2.26.0
 *@version:  v1.0.0
 *@describe: Implement the encapsulation of dynamic application memory by thread manager
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190729         EtcFly          Create  file
**********************************************************************************************************/
#ifndef _THREADM_MEM_H
#define _THREADM_MEM_H












#ifdef _cplusplus
extern "C" {
#endif





void *threadM_malloc(rt_size_t nbyte);
void *threadM_calloc(rt_size_t count, rt_size_t size);
void *threadM_relloc(void *rmem, rt_size_t newsize);
void threadM_free(void *rmem);








#ifdef _cplusplus
}
#endif //_cplusplus
#endif // _THREADM_MEM_H





