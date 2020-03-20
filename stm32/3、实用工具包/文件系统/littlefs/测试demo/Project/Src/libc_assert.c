/**********************************************************************************************************
 *@fileName: libc_assert.c
 *@platform: stm32l4xx + mdk5.26.2.0
 *@version:  v1.0.0
 *@describe: c assert library
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190606          wy             Create  file
**********************************************************************************************************/
#include "libc_assert.h"








void libc_assertCallback(unsigned char flag, void *pfunc(void *p_arg))
{
	if (!flag) {
		libc_printf("assert callback function %p\r\n", pfunc);
		pfunc((void *)0);
	}
}


