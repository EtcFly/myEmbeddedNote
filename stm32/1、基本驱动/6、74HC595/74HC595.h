/**************************************************************************************************************************
*@fileName: 74HC595.h
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: 595 device c interface file 
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190225                                           Create File
===========================================================================================================================
**************************************************************************************************************************/
#ifndef _74HC595_H
#define _74HC595_H


#include "bitband.h"


//#define MAX_CODE_SIZE  16  //��ʾ�Ķ�����
//#define MAX_LOCAT_SIZE 8  //�������
#define   SERIAL_595_NUM         3 //������595��Ŀ

#define RCC_DAT_CLK   RCC_APB2Periph_GPIOB
#define RCC_RCK_CLK   RCC_APB2Periph_GPIOB
#define RCC_SCK_CLK   RCC_APB2Periph_GPIOC

#define GPIO_DAT     GPIOB
#define GPIO_RCK      GPIOB
#define GPIO_SCK      GPIOC

#define PIN_DAT      GPIO_Pin_15
#define PIN_SCK       GPIO_Pin_8
#define PIN_RCK       GPIO_Pin_14

#define DAT    PBout(15)
#define RCK    PBout(14)
#define SCK    PCout(8)


#define HC595_DAT     DAT
#define HC595_CLK     SCK
#define HC595_LATCH   RCK
 

void HC595_Write(uint8_t *data, uint8_t nByte);
void HC595_GPIOConfig(void);

#endif




