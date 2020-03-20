/**=============================================================================
* @ModuleName : SERVO_V1.1_PCB
* @FileName   : drv_ad5663r.h
* @ARMVersion : 1.0
* @Author     : Garfield LV
* @Description: AD5663R driver pin map.
* @Notes      :
* @History    :
* -----------------------------------------------------------------------------
*	Version 1.1 - 2017.11.29 By Garfield Lv
*	[Bug Fix]
*	<Description> xxx.
*	<Modify> xxx;
* -----------------------------------------------------------------------------
*	Version 1.0 - 2017.05.22 By Garfield Lv
*	File created.
*==============================================================================*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_AD5663R_H
#define __DRV_AD5663R_H
#include "sys.h"

/* Exported constants --------------------------------------------------------*/
//#define	DA_Fine_Cmd			(0x00)
//#define	DA_Coarse_Cmd		(0x01)
//#define	DA_IniRef_Cmd		(0x38)
//#define	DA_IniRefOn_Data	(0x0001)

#define AD5663R_Cmd_0       (u8	)((0x00) << 3)    //write to input register n
#define AD5663R_Cmd_1       (u8 )((0x01) << 3)    //update DAC register n
#define AD5663R_Cmd_2       (u8 )((0x02) << 3)    //write to input register n, update all(software LDAC)
#define AD5663R_Cmd_3       (u8 )((0x03) << 3)    //write to and update DAC channel n
#define AD5663R_Cmd_4       (u8 )((0x04) << 3)    //power down DAC(power up)
#define AD5663R_Cmd_5       (u8 )((0x05) << 3)    //reset
#define AD5663R_Cmd_6       (u8 )((0x06) << 3)    //LDAC register setup
#define AD5663R_Cmd_7       (u8 )((0x07) << 3)    //internal reference setup(on/off)

#define AD5663R_ADDR_0      (u8 )(0x00)   //DAC_A
#define AD5663R_ADDR_1      (u8 )(0x01)   //DAC_B
#define AD5663R_ADDR_7      (u8 )(0x07)   //ALL DAC

#define AD5663R_DATA_INREFON  (u16 )(0x01)
#define AD5663R_DATA_INREFOFF (u16 )(0x00)

#define AD5663R_DATA_LDACON   (u16 )(0x03)

#define	AD5663R_COM_Wrt_1Ch				AD5663R_Cmd_0
#define	AD5663R_COM_Wrt_1Ch_UpdateAll	AD5663R_Cmd_2
#define	AD5663R_COM_Wrt_Update_1Ch		AD5663R_Cmd_3
#define	AD5663R_COM_RESET				AD5663R_Cmd_5
#define	AD5663R_COM_LDAC				AD5663R_Cmd_6
#define	AD5663R_COM_REF					AD5663R_Cmd_7

#define AD5663R_ComCombine(x,y)     (x | y)

/* AD5663R	DA_SERVO GPIO ----------------------------------------------------------------- */
#define	GPIO_DA_SERVO_LDAC		GPIOA
#define	PIN_DA_SERVO_LDAC		GPIO_Pin_5
#define	DA_SERVO_LDAC_Set()		GPIO_SetBits(GPIO_DA_SERVO_LDAC,PIN_DA_SERVO_LDAC)
#define	DA_SERVO_LDAC_Rst()		GPIO_ResetBits(GPIO_DA_SERVO_LDAC,PIN_DA_SERVO_LDAC)

#define	GPIO_DA_SERVO_SYNC		GPIOA
#define	PIN_DA_SERVO_SYNC		GPIO_Pin_8
#define	DA_SERVO_SYNC_Set()		GPIO_SetBits(GPIO_DA_SERVO_SYNC,PIN_DA_SERVO_SYNC)
#define	DA_SERVO_SYNC_Rst()		GPIO_ResetBits(GPIO_DA_SERVO_SYNC,PIN_DA_SERVO_SYNC)

#define	GPIO_DA_SERVO_MOSI		GPIOA
#define	PIN_DA_SERVO_MOSI		GPIO_Pin_6
#define	DA_SERVO_MOSI_Set()		GPIO_SetBits(GPIO_DA_SERVO_MOSI,PIN_DA_SERVO_MOSI)
#define	DA_SERVO_MOSI_Rst()		GPIO_ResetBits(GPIO_DA_SERVO_MOSI,PIN_DA_SERVO_MOSI)

#define	GPIO_DA_SERVO_SCK		GPIOA
#define	PIN_DA_SERVO_SCK		GPIO_Pin_7
#define	DA_SERVO_SCK_Set()		GPIO_SetBits(GPIO_DA_SERVO_SCK,PIN_DA_SERVO_SCK)
#define	DA_SERVO_SCK_Rst()		GPIO_ResetBits(GPIO_DA_SERVO_SCK,PIN_DA_SERVO_SCK)

/* -------------------------------------------------------------------------------------- */

void			DRV_AD5663R_Set(u8 com, u8 addr, u16 dadata);
void			DRV_AD5663R_Initial(void);
void			DRV_AD5663R_SendData(u32 sendData);
void            Drv_AD5563R_PinConfig(void);
void			DRV_AD5663R_Reset(void);
void 			Drv_AD5563R_Data(uint16_t dacData);
#endif
