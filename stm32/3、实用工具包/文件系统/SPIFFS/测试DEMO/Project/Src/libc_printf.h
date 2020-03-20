/**********************************************************************************************************
 *@fileName: libc_printf.h
 *@platform: stm32l4xx.h + mdk5.26.2.0
 *@version:  v1.0.0
 *@describe: the lib of printf function 
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190605          wy             Create  file
**********************************************************************************************************/
#ifndef _SYS_PRINTF_H
#define _SYS_PRINTF_H


#ifdef _cplusplus
extern "C" {
#endif

//#include "stdarg.h"

#define    MAX_PRINTF_BUFFER_SIZE                          (128u)


#ifndef va_start
typedef unsigned int size_t;
typedef char * va_list; 

#define __INCREASE_AP(x)          ((sizeof(x)+sizeof(int)-1) & (~(sizeof(int)-1)))	
#define va_start(ap, fmt)         (ap = ((va_list)&fmt) + __INCREASE_AP(fmt))
#define va_arg(ap, type)          (*(type *)((ap += __INCREASE_AP(type)) - __INCREASE_AP(type)))
#define va_end(ap)                (ap = (va_list)0)

#endif

#define    SYS_RETURN_ERR                      (-1)
#define    SYS_EOF                             SYS_RETURN_ERR
#define    SYS_NULL                            ((void *)0)



/*************************EXPORT FUNCTION***********************************/
int libc_vsnprintf (char * buf, size_t size, const char * fmt, va_list args);
int libc_vsprintf(char *str, const char *fmt, va_list args);
int libc_snprintf(char *buf, size_t size, const char *fmt, ...);
void libc_printf(const char *fmt, ...);
int libc_putchar(int ch);
int libc_getchar(void);
int libc_scanf(const char * restrict format,...);
int libc_puts(const char *string);
char *libc_gets(char *str);




#ifdef _cplusplus
}
#endif






#endif

/*****************************END OF FILE*********************************/
