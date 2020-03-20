
#ifndef _BSP_FLASH_H
#define _BSP_FLASH_H


#include <stm32l4xx_hal.h>

#define PAGE_SIZE                       (2*1024)
#define FLASH_TOTAL_SIZE                (128*1024)
#define MIN_PCROP_SIZE					(8u)  //Proprietary code readout protection 
#define MIN_WRP_SIZE                    (0X800) //min write protect area size 

typedef enum
{	
	ERR_OK,
	ERR_READ,
	ERR_WRITE,
	ERR_ERASE,
	ERR_TIMOUT,
}FLASH_Status;


FLASH_Status flash_write(uint32_t addr, uint64_t data);
FLASH_Status flash_read(uint32_t addr, uint8_t *const data, uint8_t size);
FLASH_Status flash_erasePage(uint32_t pageAddr);
FLASH_Status flash_eraseChip(void);
FLASH_Status flash_writeWRP(uint32_t startAddr, uint32_t endAddr);
FLASH_Status flash_writeRDP(void);
FLASH_Status flash_writePCROP(uint32_t startAddr, uint32_t endAddr);
FLASH_Status Flash_writeUnPCROP(void);
uint64_t flash_readOP(uint16_t opN);
FLASH_Status Flash_writeOTP(uint16_t offest, const uint8_t *buf, uint16_t size);
FLASH_Status Flash_readOTP(uint16_t offest, uint8_t *buf, uint16_t size);





#endif

