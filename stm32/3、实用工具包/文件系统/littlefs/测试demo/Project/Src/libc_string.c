/**********************************************************************************************************
 *@fileName: libc_string.c
 *@platform: stm32l4xx.h + mdk5.26.2.0
 *@version:  v1.0.0
 *@describe: the lib of string function 
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190606          wy             Create  file
**********************************************************************************************************/
#include "libc_string.h"


void *libc_memcpy(void *dst, const void *src, unsigned int count)
{
    char *pdst = (char *)dst, *psrc = (char *)src;
	
	if ((char *)0 == psrc || (char *)0 == pdst)
	{
		return (void *)0;
	}
	
	while (count--)
	{
		*pdst++ = *psrc++;
	}
    return dst;
}

void *libc_memmove(void *dest, const void *src, unsigned int n)
{
    char *tmp = (char *)dest, *s = (char *)src;

    if (s < tmp && tmp < s + n)
    {
        tmp += n;
        s += n;

        while (n--)
            *(--tmp) = *(--s);
    }
    else
    {
        while (n--)
            *tmp++ = *s++;
    }

    return dest;
}

int libc_memcmp(const void *cs, const void *ct, unsigned int count)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;

    return res;
}


char * libc_strcpy(char *dst, const char *src)
{
	char *pdst = dst;
	while ((*pdst++ = *src++) != '\0') { 
		;
	}
	return dst;
}

char *libc_strncpy(char *dst, const char *src, unsigned int n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                /* NUL pad the remaining n-1 bytes */
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }

    return (dst);
}

int libc_strcmp(const char *src1, const char *src2)
{
    while (*src1 && *src1 == *src2)
        src1++, src2++;

    return (*src1 - *src2);
}



int libc_strncmp(const char *cs, const char *ct, unsigned int count)
{
    register signed char __res = 0;

    while (count)
    {
        if ((__res = *cs - *ct++) != 0 || !*cs++)
            break;
        count --;
    }

    return __res;
}


unsigned int libc_strlen(const char *str)
{
	unsigned int len = 0;
	while ('\0' != *str++) 
	{
		len++;
	}
	return len;
}

 char *libc_strcat(char *dst, const char *src) 
{
	char *pdst = dst;
	if ((char  *)0 == pdst)
	{
		return (void *)0;
	}
	
	while ('\0' != *pdst) {
		pdst++;
	}
	
	while ((*pdst++ = *src++) != '\0') { 
		;
	}
	return dst;
}


char *libc_strstr(const char *s1, const char *s2)
{
    int l1, l2;

    l2 = libc_strlen(s2);
    if (!l2)
        return (char *)s1;
    l1 = libc_strlen(s1);
    while (l1 >= l2)
    {
        l1 --;
        if (!libc_memcmp(s1, s2, l2))
            return (char *)s1;
        s1 ++;
    }

    return (void *)0;
}
