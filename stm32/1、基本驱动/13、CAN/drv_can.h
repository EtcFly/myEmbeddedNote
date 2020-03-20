/**********************************************************************************************************
 *@fileName: drv_can.h
 *@platform: MDK5.26.2.0 + STM32L432KB
 *@version:  v1.0.1
 *@describe:  board support packet of can driver  
 *@note:
**********************************************************************************************************
 version          date          Author          modification                                  
----------------------------------------------------------------------------------------------------------
  v1.0.0         180330          wy             Create  file
  v1.0.1         190511          wy             portable the code to stm32f432kb and add the debug support
**********************************************************************************************************/
#ifndef _BSP_CAN_H
#define _BSP_CAN_H


#define  CAN_ERROR_INTERRUPT_ENABLE //enable error interrupt
#define  CAN_DEBUG_ENABLE           //DEBUG SUPPORT



typedef struct
{
	uint32_t dev_id;
	int8_t   data[8];
	uint8_t  data_len;
}CAN_Dev_Typedef;



void bsp_canInit(void);
int8_t bsp_canReceive(CAN_Dev_Typedef *rx);
int8_t bsp_canSend(CAN_Dev_Typedef *send);







#endif
