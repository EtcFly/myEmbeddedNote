/**********************************************************************************************************
 *@fileName: libc_math.c
 *@platform: stm32l4xx.h + mdk5.26.2.0
 *@version:  v1.0.0
 *@describe: the lib of math function 
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190606          wy             Create  file
**********************************************************************************************************/
#include "libc_math.h"







int libc_atoi(const char *str)
{
	char  len=0;
	const char *pstr = str;
	
	while (*pstr++) {
		len++;
	}
	
	if (0 == len ) {
		return 0;
	}
	
	pstr = str;
	int value=0;
	while (len--)
	{
		if (*pstr < '0' || *pstr > '9')	{
			return 0;
		}
		value = value * 10 + *pstr++ - '0';
	}
	return value;
}





