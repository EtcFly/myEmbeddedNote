/**************************************************************************************************************************
*@fileName: 24cxx.h
*@platform: MDK 5.10.0.0 + STM32F103C8T6
*@Version:  V1.0.0
*@describe: eeprom 24cxx相关驱动
*@modification
*==========================================================================================================================
* version    Author   Data                                            Details
*--------------------------------------------------------------------------------------------------------------------------
* V1.0.0      Wy     190222                                           Create File
===========================================================================================================================
**************************************************************************************************************************/

#ifndef _24CXX_H
#define _24CXX_H






#define   DEVICE_24CXX_ADDR                          0XA0     //设备地址
#define   MAX_24CXX_SIZE                             ((8*1024u))                       
#define   MAX_24CXX_PAGE_BUFF_SIZE                   (32u) //32Byte
//i2c pin config
#define   RCC_EEPROM_I2C_SCL                         RCC_APB2Periph_GPIOA
#define   GPIO_EEPROM_I2C_SCL                        GPIOA
#define   PIN_EEPROM_I2C_SCL                         GPIO_Pin_0


#define   RCC_EEPROM_I2C_SDA                         RCC_APB2Periph_GPIOA
#define   GPIO_EEPROM_I2C_SDA                        GPIOA
#define   PIN_EEPROM_I2C_SDA                         GPIO_Pin_1




//void eeprom_24cxx_init(void);
//uint8_t eeprom_24cxx_readBytes(uint8_t *const buff, uint32_t len, uint8_t devAddr, uint32_t addr);
//uint8_t eeprom_24cxx_writeBytes(const uint8_t *buff, uint32_t len, uint8_t devAddr, uint32_t addr);
//void eeprom_write(const uint8_t *buff, uint32_t len, uint8_t devAddr, uint32_t startAddr);
//void eeprom_read(uint8_t *buff, uint32_t len, uint8_t devAddr, uint32_t startAddr);

//application interface
void flash_write(const uint8_t *pPara, uint32_t startAddr, uint32_t len);
void flash_read(uint8_t *const pPara, uint32_t startAddr, uint32_t len);
void flash_init(uint8_t *const prdBaseAddr, uint32_t readSize);
void flash_erase(uint8_t devAddr);




#endif

