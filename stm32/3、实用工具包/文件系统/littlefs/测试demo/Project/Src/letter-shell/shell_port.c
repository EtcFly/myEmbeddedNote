
#include "stm32f1xx_hal.h"
#include "shell_port.h"
#include "bsp_uart.h"

SHELL_TypeDef shell;


void low_shellWrite(const char ch)
{
	ComSend(COM1, (uint8_t *)&ch, 1);
}

int8_t low_shellRead(char *ch)
{
	if (ComGet(COM1, (uint8_t *)ch) != 0)
	{
		return 0;
	}
	return -1;
}

void shellInitAll(void)
{
	bsp_InitCOM();
	shellInit(&shell, low_shellRead, low_shellWrite);
}














