/**************************************************************************************************************************
*@fileName: 74HC595.c
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: 595 device c file
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190225                                           Create File
===========================================================================================================================
**************************************************************************************************************************/
#include "74HC595.h"
#include "stm32f10x.h"

//#define MAX_CODE_SIZE  16
//const u8 displayCode[MAX_CODE_SIZE+1] = {0X05, ~0X42, 0X86, 0X94, 0X3C, 0X54, 0X44, ~0X62, 0X04, 0X14, 0X0C, 0X63, 0X47, 0XA4, 0X46, 0X4E, 0XFF};
//const u8 locatCode[MAX_LOCAT_SIZE] = {0XFE, 0XFD, 0XFB, 0XF7, 0XEF, 0XDF, 0XBF, 0X7F};


void Delay_595(void)
{
	uint8_t i;
	/*　
	 	下面的时间是通过安富莱AX-Pro逻辑分析仪测试得到的。
		CPU主频72MHz时，在内部Flash运行, MDK工程不优化
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
        
    IAR工程编译效率高，不能设置为7
	*/
	
	for (i = 0; i < 3; i++);
}

//595 gpio config
void HC595_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_DAT_CLK | RCC_SCK_CLK | RCC_RCK_CLK, ENABLE);

    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = PIN_DAT;
    GPIO_Init(GPIO_DAT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = PIN_SCK;
    GPIO_Init(GPIO_SCK, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = PIN_RCK;
    GPIO_Init(GPIO_RCK, &GPIO_InitStruct);
}


//函数功能：通过595串口写入字节并并行输出
//参数：dataByte写入的字节  
void HC595_Write(uint8_t *data, uint8_t nByte)
{	
    u8 i, j;

    HC595_LATCH = 0;
    for (j=0; j<nByte; j++)
    for (i = 0; i < 8; i++)
    {
        HC595_CLK = 0;
        Delay_595();
        HC595_DAT = data[j] & 0x01; //低位先传
        HC595_CLK = 1;
        data[j] >>= 1;
        Delay_595();
    } 
    HC595_LATCH = 0;
    HC595_LATCH = 1;
}




