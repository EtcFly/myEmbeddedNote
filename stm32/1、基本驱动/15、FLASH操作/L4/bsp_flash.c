/**********************************************************************************************************
 *@fileName: bsp_flash.c
 *@platform: stm32l432kbxx + MDK5.26.2.0
 *@version:  v1.0.0
 *@describe: the flash driver of stm32f4xx
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         190529          wy             Create  file
**********************************************************************************************************/
#include "bsp_flash.h"

//user application sector start address
#define  FLASH_BASE_ADDR                       (0X08000000UL)
#define  FLASH_END_ADDR                        (FLASH_BASE_ADDR+FLASH_TOTAL_SIZE)

 //system information block
#define  FLASH_INFO_BASE                       (0X1FFF0000UL)
#define  FLASH_SYS_MEM_BASE                    (FLASH_INFO_BASE+0*1024)
#define  FLASH_SYS_MEM_END                     (FLASH_INFO_BASE+28*1024)

//otp area end address
#define  FLASH_OTP_BASE                        (0X1FFF7000UL)
#define  FLASH_OTP_END                         (FLASH_OTP_BASE+1*1024)  

 //option byte end address
#define  FLASH_OP_BASE                         (0X1FFF7800UL)
#define  FLASH_OP_END                          (FLASH_OP_BASE+16) 


//flash read latency 
#define  FLASH_LATENCY_0WS                     0X0000
#define  FLASH_LETENCY_1WS                     0X0001
#define  FLASH_LETENCY_2WS					   0X0002
#define  FLASH_LETENCY_3WS					   0X0003
#define  FLASH_LETENCY_4WS					   0X0004
#define  FLASH_LETENCY_RSV                     0X000F


typedef enum
{
	OPER_OK,
	OPER_ERR,
}Oper_Status;


static void flash_getOption(FLASH_OBProgramInitTypeDef *pProgram)
{
	if (NULL != pProgram) 
	{
		pProgram->WRPArea = OB_WRPAREA_BANK1_AREAA;
		pProgram->PCROPConfig = FLASH_BANK_1;
		HAL_FLASHEx_OBGetConfig(pProgram);	
	}
}

FLASH_Status flash_write(uint32_t addr, uint64_t data)
{
	FLASH_Status status = ERR_OK;
	if (addr % 8 != 0) {
		return ERR_WRITE;
	}
	
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, data) != HAL_OK) {
		status = ERR_WRITE;
	}
	HAL_FLASH_Lock();

	return status;
}

FLASH_Status flash_read(uint32_t addr, uint8_t *const data, uint8_t size)
{
	FLASH_Status status = ERR_OK;
	uint8_t i;
	for (i=0; i<size; i++)
	{
		data[i] = *((volatile uint8_t *)(addr+i));
	}
	return status;
}

FLASH_Status flash_erasePage(uint32_t pageAddr)
{
	FLASH_Status status = ERR_OK;
	FLASH_EraseInitTypeDef erase;
	uint32_t               errCode;
	
	if (pageAddr < FLASH_BASE_ADDR || pageAddr >= FLASH_END_ADDR) {
		return ERR_ERASE;
	}
	
	erase.Banks = FLASH_BANK_1;
	erase.NbPages = 1;
	erase.Page = (pageAddr - FLASH_BASE_ADDR) / PAGE_SIZE;
	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	if (HAL_FLASHEx_Erase(&erase, &errCode) != HAL_OK)
	{
		status = ERR_ERASE;
	}
	HAL_FLASH_Lock();
	return status;
}

FLASH_Status flash_eraseChip(void)
{
	FLASH_Status status = ERR_OK;	
	FLASH_EraseInitTypeDef erase;
	uint32_t               errCode;
	
	erase.Page = 0;
	erase.Banks = FLASH_BANK_1;
	erase.NbPages = FLASH_TOTAL_SIZE;
	erase.TypeErase = FLASH_TYPEERASE_MASSERASE;
	
	HAL_FLASH_Unlock();	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	if (HAL_FLASHEx_Erase(&erase, &errCode) != HAL_OK){
		status = ERR_ERASE;
	}
	HAL_FLASH_Lock();
	return status;
}

//write flash write protect
FLASH_Status flash_writeWRP(uint32_t startAddr, uint32_t endAddr)
{
	FLASH_Status status = ERR_OK;
	FLASH_OBProgramInitTypeDef obProgram;
	HAL_StatusTypeDef result;
	uint32_t startOffest, endOffest;
	
	if (endAddr < startAddr) {
		return ERR_WRITE;
	}
	
	if (startAddr < FLASH_BASE_ADDR || endAddr > FLASH_END_ADDR) {
		return ERR_WRITE;	
	}	
	
	startOffest = (startAddr - FLASH_BASE_ADDR) / MIN_WRP_SIZE;
	endOffest = (endAddr - FLASH_BASE_ADDR) / MIN_WRP_SIZE;
	
	flash_getOption(&obProgram);
	if (obProgram.WRPArea != OB_WRPAREA_BANK1_AREAA || obProgram.WRPStartOffset != startOffest || obProgram.WRPEndOffset != endOffest) {	
		obProgram.OptionType = OPTIONBYTE_WRP;
		obProgram.WRPArea = OB_WRPAREA_BANK1_AREAA;
		obProgram.WRPEndOffset = endOffest;
		obProgram.WRPStartOffset = startOffest;
		
		HAL_FLASH_Unlock();
		HAL_FLASH_OB_Unlock();
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
		result = HAL_FLASHEx_OBProgram(&obProgram);
		if (result != HAL_OK) {
			status = ERR_WRITE;
		} else {
			if (HAL_FLASH_OB_Launch() != HAL_OK) { //it will generate a load option byte reset at frist run time
				status = ERR_WRITE;
			}	
		}
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
	}
	return status;
}

//write flash read protect
FLASH_Status flash_writeRDP(void)
{
	FLASH_Status status = ERR_OK;
	FLASH_OBProgramInitTypeDef obProgram;
	HAL_StatusTypeDef result;
	
	flash_getOption(&obProgram);
	if (OB_RDP_LEVEL_0 == obProgram.RDPLevel) {
		obProgram.OptionType = OPTIONBYTE_RDP;
		obProgram.RDPLevel = OB_RDP_LEVEL_1;
		
		HAL_FLASH_Unlock();
		HAL_FLASH_OB_Unlock();
		result = HAL_FLASHEx_OBProgram(&obProgram);
		if (result != HAL_OK) {
			status = ERR_WRITE;
		} else {
			if (HAL_FLASH_OB_Launch() != HAL_OK) {
				status = ERR_WRITE;
			}
		}
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
	}
	
	return status;
}

//read option byte and the parameter is between 0 to 15
uint64_t flash_readOP(uint16_t opN)
{
	uint64_t data = 0X00;
	
	if (opN >= 16) {
		return 0X00;
	}
	data = *((uint64_t *)(FLASH_OP_BASE+(opN<<3)));
	return data;
}

FLASH_Status flash_writePCROP(uint32_t startAddr, uint32_t endAddr)
{
	FLASH_Status status = ERR_OK;
	FLASH_OBProgramInitTypeDef obProgram;
	
	if (endAddr < startAddr || endAddr - startAddr < MIN_PCROP_SIZE) {
		return ERR_WRITE;
	}
	
	if (startAddr < FLASH_BASE_ADDR || endAddr > FLASH_END_ADDR) {
		return ERR_WRITE;	
	}
	
	flash_getOption(&obProgram);
	if (obProgram.RDPLevel != OB_RDP_LEVEL_1 || obProgram.PCROPStartAddr != startAddr || obProgram.PCROPEndAddr != endAddr) {
		obProgram.OptionType = OPTIONBYTE_PCROP | OPTIONBYTE_RDP;
		obProgram.PCROPConfig = OB_PCROP_RDP_ERASE | FLASH_BANK_1;
		obProgram.PCROPEndAddr = endAddr;
		obProgram.PCROPStartAddr = startAddr;
		obProgram.RDPLevel = OB_RDP_LEVEL_1;
		
		HAL_FLASH_Unlock();
		HAL_FLASH_OB_Unlock();
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
		if (HAL_FLASHEx_OBProgram(&obProgram) != HAL_OK) {
			status = ERR_WRITE;
		} else {
			if (HAL_FLASH_OB_Launch() != HAL_OK) {
				status = ERR_WRITE;
			}	
		}
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
	}
	
	return status;
}

//if unlock pcrop and it will clear all of sector
FLASH_Status Flash_writeUnPCROP(void)
{
	FLASH_Status status = ERR_OK;
	FLASH_OBProgramInitTypeDef obProgram;

	obProgram.OptionType = OPTIONBYTE_PCROP | OPTIONBYTE_RDP;
	obProgram.PCROPConfig = OB_PCROP_RDP_ERASE | FLASH_BANK_1;
	obProgram.PCROPStartAddr = 0X00;
	obProgram.PCROPEndAddr = 0X00;
	obProgram.RDPLevel = OB_RDP_LEVEL_0;
	
	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	if (HAL_FLASHEx_OBProgram(&obProgram) != HAL_OK) {
		status = ERR_WRITE;
	} else {
		if (HAL_FLASH_OB_Launch() != HAL_OK) {
			status = ERR_WRITE;
		}
	}
	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();
	
	return status;	
}

FLASH_Status Flash_writeOTP(uint16_t offest, const uint8_t *buf, uint16_t size)
{
	FLASH_Status status = ERR_OK;
	uint8_t writeLen = 8;
	uint64_t data=0x00;
	
	if (NULL == buf || offest+size > 1024)
	{
		return ERR_WRITE;
	}
	
	for (uint16_t i=0; i<size; )
	{
		if (size >= 8) {
			writeLen = 8;
		} else {
			writeLen = size;
		}		
		for (uint8_t j=0; j<writeLen; j++)
		{
			data = (data<<8) + buf[writeLen-j-1];
		}

		if (flash_write(FLASH_OTP_BASE+offest, data) != ERR_OK) {
			status = ERR_WRITE; 
			break;
		}
		data = 0;
		size -= writeLen;
		buf += writeLen;
		offest += writeLen;	
	}
	return status;
}

//the rang of offest between 0 to 1024
//the unit of size is Byte
FLASH_Status Flash_readOTP(uint16_t offest, uint8_t *buf, uint16_t size)
{
	FLASH_Status status = ERR_OK;
	uint32_t addr = (FLASH_OTP_BASE+offest);
	if (NULL == buf || offest+size>1024) {
		return ERR_READ;
	}
	
	for (uint16_t i=0; i<size; i++)
	{
		*buf++ = *(volatile uint8_t *)addr;
		addr ++;
	}
	return status;
}

