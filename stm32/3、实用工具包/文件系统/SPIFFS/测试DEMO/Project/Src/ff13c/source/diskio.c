/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sfud.h"
#include "stm32f1xx_hal.h"
#include "stdio.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


#define FLASH_FATFS_OFFEST     (0u)
#define FLASH_SECTOR_SIZE      (4096u)
#define FLASH_TOTAL_SECTOR     (4096u) //32MByte Flash

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

const sfud_flash *flash;
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
//	int result;

	switch (pdrv) {
	case DEV_RAM :
//		result = RAM_disk_status();
		if (flash != 0)
		{
			stat = RES_OK;
		} else { 
			stat = RES_ERROR;
			printf("disk status operation [error]\n");
		}
		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	sfud_err result;

	switch (pdrv) {
	case DEV_RAM :
		result = sfud_init();	
		
		// translate the reslut code here
		if (result == SFUD_SUCCESS)
		{
			flash = sfud_get_device_table() + 0;
			if (flash != 0)
			{
				stat = RES_OK;
			} else  { 
				stat = STA_NOINIT;
				printf("disk init operation [error]\n");
			}
			
		} 
		else
		{
			stat = STA_NOINIT;
		}

//	case DEV_MMC :
//		result = MMC_disk_initialize();

//		// translate the reslut code here

//		return stat;

//	case DEV_USB :
//		result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

//		while (count)
//		{
//			result = sfud_read(flash, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE, buff);
//			if (result != SFUD_SUCCESS)
//			{
//				res = RES_PARERR;
//				printf("disk read operation [error]\n");
//				break;
//			} else { 
//				buff += FLASH_SECTOR_SIZE;
//			}
//			count --;
//		}
//		
//		if (count == 0) res = RES_OK;
		// translate the reslut code here	
		result = sfud_read(flash, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE*count, buff);
		if (result != SFUD_SUCCESS)
		{
			res = RES_PARERR;
			printf("disk read operation [error]\n");
		} else res = RES_OK;
		
		return res;

//	case DEV_MMC :
//		// translate the arguments here

//		result = MMC_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

//		while (count)
//		{
//			sfud_erase(flash, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
//			result = sfud_write(flash, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE, buff);
//			if (result != SFUD_SUCCESS)
//			{
//				res = RES_PARERR;
//				printf("disk write operation [error]\n");
//				break;
//			}
//			else buff += FLASH_SECTOR_SIZE;
//			
//			count--;
//		}
//		
//		if (count == 0) res = RES_OK;
		sfud_erase(flash, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE*count);
		result = sfud_write(flash, sector*FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE*count, buff);
		if (result != SFUD_SUCCESS)
		{
			res = RES_PARERR;
			printf("disk write operation [error]\n");
		} else res = RES_OK;
	
		// translate the reslut code here
		return res;

//	case DEV_MMC :
//		// translate the arguments here

//		result = MMC_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) {
	case DEV_RAM :
		switch (cmd) 
		{
		 case CTRL_SYNC:
			 return RES_OK; 
		 
		 case GET_SECTOR_COUNT:
			 *(DWORD * )buff = FLASH_TOTAL_SECTOR;//W25Q32 有1024个大小为4k bytes 的扇区
		 		 printf("get sector count %d\n", *(DWORD * )buff);
			 return RES_OK;
 
		 /* 扇区大小 */
		 case GET_SECTOR_SIZE :
			 *(WORD * )buff = FLASH_SECTOR_SIZE;//spi flash的扇区大小是 4K Bytes
		 printf("get sector size:%d\n", *(WORD * )buff);
			 return RES_OK;
 
		 /*块大小 */
		 case GET_BLOCK_SIZE :
			 *(DWORD * )buff = 1;
		 		 printf("get block size %d\n", *(DWORD * )buff);
			 return RES_OK;
			 
		 default:
			 return RES_PARERR;
		 }
	}
	printf("disk iocal operation [error]\n");
	return RES_PARERR;
}

DWORD get_fattime (void)
{
	return HAL_GetTick();
}
