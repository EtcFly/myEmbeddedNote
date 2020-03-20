/**********************************************************************************************************
 *@fileName: libc_assert.h
 *@platform: stm32l4xx + mdk5.26.2.0
 *@version:  v1.0.0
 *@describe: c assert library
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190606          wy             Create  file
**********************************************************************************************************/
#ifndef _LIBC_ASSERT_H
#define _LIBC_ASSERT_H

#ifdef _cplusplus
extern "C" {
#endif

#include "libc_printf.h"






#define libc_logAssert(x) \
if (!(x)){                      \
	libc_printf("[logAssert] Parameter "#x" Check Fail, Value is :%d\r\n", x);  \
}


#define libc_errorAssert(x)         \
if (!(x)) {                           \
	libc_printf("[errorAssert] Parameter "#x" Check Fail, Value is :%d\r\n", x);\
	while (1);    \
}


#define libc_warnAssert(x)     \
if (!(x)) {						\
	libc_printf("[warnAssert] Parameter "#x" Check Fail, Value is :%d\r\n", x); \
	return ;   \
}





#ifdef _cplusplus
}
#endif




#endif


