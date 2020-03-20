#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include <inttypes.h>
#include "stm32f10x.h"
#define		EEP_DEVICE_ADDR		0xA0   
#define		FRAM_DEVICE_ADDR	0xA2 

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);
void i2c_CfgGpio(void);
void i2c_Delay(void);

#endif
