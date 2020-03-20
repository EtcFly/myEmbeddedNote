/**********************************************************************************************************
 *@fileName: libc_string.c
 *@platform: stm32l4xx.h + mdk5.26.2.0
 *@version:  v1.0.0
 *@describe: the lib of string function  interface
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190606          wy             Create  file
**********************************************************************************************************/
#ifndef _LIBC_STRING_H
#define _LIBC_STRING_H


#ifdef _cplusplus
extern "C" {
#endif






/**********************EXPORT FUCTION**************************/
//mem
void *libc_memcpy(void *dst, const void *src, unsigned int count);
void *libc_memmove(void *dest, const void *src, unsigned int n);
int libc_memcmp(const void *cs, const void *ct, unsigned int count);

//string 
unsigned int sys_strlen(const char *str);
char * libc_strcpy(char *dst, const char *src);
char *libc_strncpy(char *dst, const char *src, unsigned int n);
int libc_strcmp(const char *src1, const char *src2);
int libc_strncmp(const char *cs, const char *ct, unsigned int count);unsigned int libc_strlen(const char *str);
unsigned int libc_strlen(const char *str);
char *libc_strcat(char *dest, const char *src);
char *libc_strstr(const char *s1, const char *s2);



#ifdef _cplusplus
}
#endif


#endif
